#include "lexer.h"

#include "config.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "buffer.h"
#include "cdefs.h"
#include "diff.h"
#include "error.h"
#include "options.h"
#include "token.h"
#include "util.h"
#include "vector.h"

#ifdef HAVE_QUEUE
#  include <sys/queue.h>
#else
#  include "compat-queue.h"
#endif

struct lexer {
	struct lexer_state	 lx_st;
	struct lexer_callbacks	 lx_callbacks;
	struct error		*lx_er;
	const struct options	*lx_op;
	const struct diffchunk	*lx_diff;
	const struct buffer	*lx_bf;
	const char		*lx_path;

	/* Line number to buffer offset mapping. */
	VECTOR(unsigned int)	 lx_lines;

	int			 lx_eof;
	int			 lx_peek;

	struct token		*lx_unmute;

	struct token_list	 lx_tokens;
	VECTOR(struct token *)	 lx_stamps;
	VECTOR(char *)		 lx_serialized;
};

static void	lexer_line_alloc(struct lexer *, unsigned int);

static void	lexer_expect_error(struct lexer *, int, const struct token *,
    const char *, int);

static void	lexer_branch_fold(struct lexer *, struct token *);
static void	lexer_branch_unmute(struct lexer *, struct token *);

#define LEXER_BRANCH_BACKWARD		0x00000001u
#define LEXER_BRANCH_FORWARD		0x00000002u
#define LEXER_BRANCH_INTACT		0x00000004u

static struct token	*lexer_recover_branch(struct token *);
static struct token	*lexer_recover_branch1(struct token *, unsigned int);

#define lexer_trace(lx, fmt, ...) do {					\
	if (trace((lx)->lx_op, 'l') >= 2)				\
		tracef('L', __func__, (fmt), __VA_ARGS__);		\
} while (0)

struct lexer *
lexer_alloc(const struct lexer_arg *arg)
{
	struct lexer *lx;
	int error = 0;

	lx = ecalloc(1, sizeof(*lx));
	lx->lx_callbacks = arg->callbacks;
	lx->lx_er = arg->er;
	lx->lx_op = arg->op;
	lx->lx_bf = arg->bf;
	lx->lx_diff = arg->diff;
	lx->lx_path = arg->path;
	lx->lx_st.st_lno = 1;
	lx->lx_st.st_cno = 1;
	if (VECTOR_INIT(lx->lx_lines) == NULL)
		err(1, NULL);
	TAILQ_INIT(&lx->lx_tokens);
	if (VECTOR_INIT(lx->lx_stamps) == NULL)
		err(1, NULL);
	if (VECTOR_INIT(lx->lx_serialized) == NULL)
		err(1, NULL);
	lexer_line_alloc(lx, 1);

	for (;;) {
		struct token *tk;

		tk = lx->lx_callbacks.read(lx, lx->lx_callbacks.arg);
		if (tk == NULL) {
			error = 1;
			break;
		}
		TAILQ_INSERT_TAIL(&lx->lx_tokens, tk, tk_entry);
		if (tk->tk_type == LEXER_EOF)
			break;
	}

	if (error) {
		lexer_free(lx);
		return NULL;
	}

	if (trace(lx->lx_op, 'l') >= 3)
		lexer_dump(lx);

	return lx;
}

void
lexer_free(struct lexer *lx)
{
	struct token *tk;

	if (lx == NULL)
		return;

	VECTOR_FREE(lx->lx_lines);
	if (lx->lx_unmute != NULL)
		token_rele(lx->lx_unmute);
	while (!VECTOR_EMPTY(lx->lx_stamps)) {
		tk = *VECTOR_POP(lx->lx_stamps);
		token_rele(tk);
	}
	VECTOR_FREE(lx->lx_stamps);
	while ((tk = TAILQ_FIRST(&lx->lx_tokens)) != NULL) {
		TAILQ_REMOVE(&lx->lx_tokens, tk, tk_entry);
		assert(tk->tk_refs == 1);
		token_rele(tk);
	}
	while (!VECTOR_EMPTY(lx->lx_serialized)) {
		char *str;

		str = *VECTOR_POP(lx->lx_serialized);
		free(str);
	}
	VECTOR_FREE(lx->lx_serialized);
	free(lx);
}

struct lexer_state
lexer_get_state(const struct lexer *lx)
{
	return lx->lx_st;
}

void
lexer_set_state(struct lexer *lx, const struct lexer_state *st)
{
	lx->lx_st = *st;
}

int
lexer_getc(struct lexer *lx, unsigned char *ch)
{
	const struct buffer *bf = lx->lx_bf;
	unsigned char c;

	if (lx->lx_st.st_off == bf->bf_len) {
		/*
		 * Do not immediately report EOF. Instead, return something
		 * that's not expected while reading a token.
		 */
		if (lx->lx_eof++ > 0)
			return 1;
		*ch = '\0';
		return 0;
	}
	c = (unsigned char)bf->bf_ptr[lx->lx_st.st_off++];
	if (c == '\n') {
		lx->lx_st.st_lno++;
		lx->lx_st.st_cno = 1;
		lexer_line_alloc(lx, lx->lx_st.st_lno);
	} else {
		lx->lx_st.st_cno++;
	}
	*ch = c;
	return 0;
}

void
lexer_ungetc(struct lexer *lx)
{
	if (lx->lx_eof)
		return;

	assert(lx->lx_st.st_off > 0);
	lx->lx_st.st_off--;

	if (lx->lx_bf->bf_ptr[lx->lx_st.st_off] == '\n') {
		assert(lx->lx_st.st_lno > 0);
		lx->lx_st.st_lno--;
		lx->lx_st.st_cno = 1;
	} else {
		assert(lx->lx_st.st_cno > 0);
		lx->lx_st.st_cno--;
	}
}

struct token *
lexer_emit(const struct lexer *lx, const struct lexer_state *st,
    const struct token *tk)
{
	struct token *t;

	t = token_alloc(tk);
	t->tk_off = st->st_off;
	t->tk_lno = st->st_lno;
	t->tk_cno = st->st_cno;
	if (lexer_get_diffchunk(lx, t->tk_lno) != NULL)
		t->tk_flags |= TOKEN_FLAG_DIFF;
	if (t->tk_str == NULL) {
		t->tk_str = &lx->lx_bf->bf_ptr[st->st_off];
		t->tk_len = lx->lx_st.st_off - st->st_off;
	}
	return t;
}

void
lexer_error(struct lexer *lx, const char *fmt, ...)
{
	va_list ap;
	struct buffer *bf;

	lx->lx_st.st_err++;

	va_start(ap, fmt);
	bf = error_begin(lx->lx_er);
	buffer_printf(bf, "%s: ", lx->lx_path);
	buffer_vprintf(bf, fmt, ap);
	buffer_printf(bf, "\n");
	error_end(lx->lx_er);
	va_end(ap);
}

/*
 * Serialize the given token. The returned string will be freed once
 * lexer_free() is invoked.
 */
const char *
lexer_serialize(struct lexer *lx, const struct token *tk)
{
	char **str;

	if (tk == NULL)
		return "(null)";

	str = VECTOR_ALLOC(lx->lx_serialized);
	if (str == NULL)
		err(1, NULL);
	*str = lx->lx_callbacks.serialize(tk);
	return *str;
}

unsigned int
lexer_get_error(const struct lexer *lx)
{
	return lx->lx_st.st_err;
}

/*
 * Get the buffer contents for the lines [beg, end). If end is equal to 0, the
 * line number of the last line is used.
 */
int
lexer_get_lines(const struct lexer *lx, unsigned int beg, unsigned int end,
    const char **str, size_t *len)
{
	const struct buffer *bf = lx->lx_bf;
	size_t bo, eo;

	bo = lx->lx_lines[beg - 1];
	if (end == 0)
		eo = bf->bf_len;
	else
		eo = lx->lx_lines[end - 1];
	*str = &bf->bf_ptr[bo];
	*len = eo - bo;
	return 1;
}

/*
 * Take note of the last consumed token, later used while branching and
 * recovering.
 */
void
lexer_stamp(struct lexer *lx)
{
	struct token *tk;

	tk = lx->lx_st.st_tk;
	if (tk->tk_flags & TOKEN_FLAG_STAMP)
		return;

	lexer_trace(lx, "stamp %s", lexer_serialize(lx, tk));
	tk->tk_flags |= TOKEN_FLAG_STAMP;
	token_ref(tk);
	*VECTOR_ALLOC(lx->lx_stamps) = tk;
}

/*
 * Try to recover after encountering invalid source code. Returns the index of
 * the stamped token seeked to, starting from the end. This index should
 * correspond to the number of documents that must be removed since we're about
 * to parse them again.
 */
int
lexer_recover(struct lexer *lx)
{
	struct token *seek = NULL;
	struct token *back, *br, *dst, *src;
	size_t i;
	int ndocs = 1;

	if (!lexer_back(lx, &back))
		back = TAILQ_FIRST(&lx->lx_tokens);
	lexer_trace(lx, "back %s", lexer_serialize(lx, back));
	br = lexer_recover_branch(back);
	if (br == NULL)
		return 0;

	src = br->tk_token;
	dst = br->tk_branch.br_nx->tk_token;
	lexer_trace(lx, "branch from %s to %s covering [%s, %s)",
	    lexer_serialize(lx, br),
	    lexer_serialize(lx, br->tk_branch.br_nx),
	    lexer_serialize(lx, src),
	    lexer_serialize(lx, dst));

	/*
	 * Find the offset of the first stamped token before the branch.
	 * Must be done before getting rid of the branch as stamped tokens might
	 * be removed.
	 */
	for (i = VECTOR_LENGTH(lx->lx_stamps); i > 0; i--) {
		struct token *stamp = lx->lx_stamps[i - 1];

		if (token_cmp(stamp, br) < 0)
			break;
		ndocs++;
	}

	/*
	 * Turn the whole branch into a prefix. As the branch is about to be
	 * removed, grab a reference since it's needed below.
	 */
	token_ref(br);
	lexer_branch_fold(lx, br);

	/* Find first stamped token before the branch. */
	for (i = VECTOR_LENGTH(lx->lx_stamps); i > 0; i--) {
		struct token *stamp = lx->lx_stamps[i - 1];

		if (token_cmp(stamp, br) < 0) {
			seek = stamp;
			break;
		}
	}
	token_rele(br);

	lexer_trace(lx, "seek to %s, removing %d document(s)",
	    lexer_serialize(lx, seek ? seek : TAILQ_FIRST(&lx->lx_tokens)),
	    ndocs);
	lx->lx_st.st_tk = seek;
	lx->lx_st.st_err = 0;
	return ndocs;
}

/*
 * Returns non-zero if the lexer took the next branch.
 */
int
lexer_branch(struct lexer *lx)
{
	struct token *br, *dst, *rm, *seek, *tk;

	if (!lexer_back(lx, &tk))
		return 0;
	br = token_get_branch(tk);
	if (br == NULL)
		return 0;

	dst = br->tk_branch.br_nx->tk_token;

	lexer_trace(lx, "branch from %s to %s, covering [%s, %s)",
	    lexer_serialize(lx, br),
	    lexer_serialize(lx, br->tk_branch.br_nx),
	    lexer_serialize(lx, br->tk_token),
	    lexer_serialize(lx, br->tk_branch.br_nx->tk_token));

	token_branch_unlink(br);

	rm = br->tk_token;
	for (;;) {
		struct token *nx;

		lexer_trace(lx, "removing %s", lexer_serialize(lx, rm));

		nx = token_next(rm);
		lexer_remove(lx, rm, 0);
		if (nx == dst)
			break;
		rm = nx;
	}

	/*
	 * Tell doc_token() that crossing this token must cause tokens to be
	 * emitted again. While here, disarm any previous unmute token as it
	 * might be crossed again.
	 */
	if (lx->lx_unmute != NULL) {
		lx->lx_unmute->tk_flags &= ~TOKEN_FLAG_UNMUTE;
		token_rele(lx->lx_unmute);
		lx->lx_unmute = NULL;
	}
	lexer_branch_unmute(lx, dst);

	/* Rewind to last stamped token. */
	seek = VECTOR_EMPTY(lx->lx_stamps) ? NULL : *VECTOR_LAST(lx->lx_stamps);
	lexer_trace(lx, "seek to %s",
	    lexer_serialize(lx, seek ? seek : TAILQ_FIRST(&lx->lx_tokens)));
	lx->lx_st.st_tk = seek;
	lx->lx_st.st_err = 0;
	return 1;
}

int
lexer_seek(struct lexer *lx, struct token *tk)
{
	lexer_trace(lx, "seek to %s", lexer_serialize(lx, tk));
	lx->lx_st.st_tk = token_prev(tk);
	return lx->lx_st.st_tk == NULL ? 0 : 1;
}

/*
 * Returns non-zero if the current token denotes a branch continuation.
 */
int
lexer_is_branch(const struct lexer *lx)
{
	struct token *tk;

	return lexer_back(lx, &tk) && token_get_branch(tk) != NULL;
}

int
lexer_pop(struct lexer *lx, struct token **tk)
{
	struct lexer_state *st = &lx->lx_st;

	if (st->st_tk == NULL) {
		st->st_tk = TAILQ_FIRST(&lx->lx_tokens);
	} else if (st->st_tk->tk_type != LEXER_EOF) {
		struct token *br;

		/* Do not move passed a branch. */
		if (lx->lx_peek == 0 && token_is_branch(st->st_tk))
			return 0;

		st->st_tk = token_next(st->st_tk);
		if (st->st_tk == NULL)
			return 1;
		br = token_get_branch(st->st_tk);
		if (br == NULL)
			goto out;

		if (lx->lx_peek == 0) {
			/* While not peeking, instruct the parser to halt. */
			lexer_trace(lx, "halt %s",
			    lexer_serialize(lx, st->st_tk));
			return 0;
		} else {
			/* While peeking, act as taking the current branch. */
			while (br->tk_branch.br_nx != NULL)
				br = br->tk_branch.br_nx;
			st->st_tk = br->tk_token;
		}
	}

out:
	*tk = st->st_tk;
	return 1;
}

/*
 * Get the last consumed token. Returns non-zero if such token is found.
 */
int
lexer_back(const struct lexer *lx, struct token **tk)
{
	if (lx->lx_st.st_tk == NULL)
		return 0;
	*tk = lx->lx_st.st_tk;
	return 1;
}

struct token *
lexer_copy_after(struct lexer *lx, struct token *after, const struct token *src)
{
	struct token *tk;

	tk = token_alloc(src);
	TAILQ_INSERT_AFTER(&lx->lx_tokens, after, tk, tk_entry);
	return tk;
}

struct token *
lexer_insert_before(struct lexer *UNUSED(lx), struct token *before, int type,
    const char *str)
{
	const struct token cp = {
		.tk_type	= type,
		.tk_lno		= before->tk_lno,
		.tk_cno		= before->tk_cno,
		.tk_flags	= token_flags_inherit(before),
		.tk_str		= str,
		.tk_len		= strlen(str),
	};
	struct token *tk;

	tk = token_alloc(&cp);
	TAILQ_INSERT_BEFORE(before, tk, tk_entry);
	return tk;
}

struct token *
lexer_insert_after(struct lexer *lx, struct token *after, int type,
    const char *str)
{
	const struct token cp = {
		.tk_type	= type,
		.tk_lno		= after->tk_lno,
		.tk_cno		= after->tk_cno,
		.tk_flags	= token_flags_inherit(after),
		.tk_str		= str,
		.tk_len		= strlen(str),
	};
	struct token *tk;

	tk = token_alloc(&cp);
	TAILQ_INSERT_AFTER(&lx->lx_tokens, after, tk, tk_entry);
	return tk;
}

struct token *
lexer_move_after(struct lexer *lx, struct token *after, struct token *tk)
{
	TAILQ_REMOVE(&lx->lx_tokens, tk, tk_entry);
	tk->tk_lno = after->tk_lno;
	tk->tk_cno = after->tk_cno + after->tk_len;
	TAILQ_INSERT_AFTER(&lx->lx_tokens, after, tk, tk_entry);
	return tk;
}

struct token *
lexer_move_before(struct lexer *lx, struct token *before, struct token *tk)
{
	TAILQ_REMOVE(&lx->lx_tokens, tk, tk_entry);
	tk->tk_lno = before->tk_lno;
	tk->tk_cno = before->tk_cno;
	TAILQ_INSERT_BEFORE(before, tk, tk_entry);
	return tk;
}

void
lexer_remove(struct lexer *lx, struct token *tk, int keepfixes)
{
	assert(tk->tk_type != LEXER_EOF);

	if (keepfixes) {
		struct token *nx, *pv;

		/*
		 * Next token must always be present, we're in trouble while
		 * trying to remove the EOF token which is the only one lacking
		 * a next token.
		 */
		nx = token_next(tk);
		assert(nx != NULL);
		token_move_prefixes(tk, nx);

		pv = token_prev(tk);
		if (pv == NULL)
			pv = nx;
		token_move_suffixes(tk, pv);
	} else {
		struct token *fix;

		TAILQ_FOREACH(fix, &tk->tk_prefixes, tk_entry)
			token_branch_unlink(fix);
	}

	if (tk->tk_flags & TOKEN_FLAG_STAMP) {
		size_t i;

		tk->tk_flags &= ~TOKEN_FLAG_STAMP;
		token_rele(tk);
		for (i = 0; i < VECTOR_LENGTH(lx->lx_stamps); i++) {
			if (lx->lx_stamps[i] == tk)
				break;
		}
		for (i++; i < VECTOR_LENGTH(lx->lx_stamps); i++)
			lx->lx_stamps[i - 1] = lx->lx_stamps[i];
		VECTOR_POP(lx->lx_stamps);
	}

	if (tk->tk_flags & TOKEN_FLAG_UNMUTE) {
		assert(tk == lx->lx_unmute);
		tk->tk_flags &= ~TOKEN_FLAG_UNMUTE;
		token_rele(tk);
		lx->lx_unmute = NULL;
	}

	token_list_remove(&lx->lx_tokens, tk);
}

int
lexer_expect0(struct lexer *lx, int type, struct token **tk,
    const char *fun, int lno)
{
	struct token *t = NULL;

	if (!lexer_if(lx, type, &t)) {
		/* Peek at the next token to provide meaningful errors. */
		lexer_peek(lx, &t);
		lexer_expect_error(lx, type, t, fun, lno);
		return 0;
	}
	if (tk != NULL)
		*tk = t;
	return 1;
}

void
lexer_peek_enter(struct lexer *lx, struct lexer_state *st)
{
	*st = lx->lx_st;
	lx->lx_peek++;
}

void
lexer_peek_leave(struct lexer *lx, const struct lexer_state *st)
{
	lx->lx_st = *st;
	assert(lx->lx_peek > 0);
	lx->lx_peek--;
}

/*
 * Peek at the next token without consuming it. Returns non-zero if such token
 * was found.
 */
int
lexer_peek(struct lexer *lx, struct token **tk)
{
	struct lexer_state s;
	struct token *t = NULL;
	int pop;

	lexer_peek_enter(lx, &s);
	pop = lexer_pop(lx, &t);
	lexer_peek_leave(lx, &s);
	if (!pop)
		return 0;
	if (tk != NULL)
		*tk = t;
	return 1;
}

/*
 * Peek at the next token without consuming it only if it matches the given
 * type. Returns non-zero if such token was found.
 */
int
lexer_peek_if(struct lexer *lx, int type, struct token **tk)
{
	struct token *t;

	if (lexer_peek(lx, &t) && t->tk_type == type) {
		if (tk != NULL)
			*tk = t;
		return 1;
	}
	return 0;
}

/*
 * Consume the next token if it matches the given type. Returns non-zero if such
 * token was found.
 */
int
lexer_if(struct lexer *lx, int type, struct token **tk)
{
	struct token *t;

	if (!lexer_peek_if(lx, type, tk) || !lexer_pop(lx, &t))
		return 0;
	return 1;
}

/*
 * Peek at the flags of the next token without consuming it. Returns non-zero if
 * such token was found.
 */
int
lexer_peek_if_flags(struct lexer *lx, unsigned int flags, struct token **tk)
{
	struct token *t;

	if (!lexer_peek(lx, &t) || (t->tk_flags & flags) == 0)
		return 0;
	if (tk != NULL)
		*tk = t;
	return 1;
}

/*
 * Consume the next token if it matches the given flags. Returns non-zero such
 * token was found.
 */
int
lexer_if_flags(struct lexer *lx, unsigned int flags, struct token **tk)
{
	struct token *t;

	if (!lexer_peek_if_flags(lx, flags, &t) || !lexer_pop(lx, &t))
		return 0;
	if (tk != NULL)
		*tk = t;
	return 1;
}

/*
 * Peek at the next balanced pair of tokens such as parenthesis or squares.
 * Returns non-zero if such tokens was found.
 */
int
lexer_peek_if_pair(struct lexer *lx, int lhs, int rhs, struct token **tk)
{
	struct lexer_state s;
	struct token *t = NULL;
	int pair = 0;

	if (!lexer_peek_if(lx, lhs, NULL))
		return 0;

	lexer_peek_enter(lx, &s);
	for (;;) {
		if (!lexer_pop(lx, &t))
			break;
		if (t->tk_type == LEXER_EOF)
			break;
		if (t->tk_type == lhs)
			pair++;
		if (t->tk_type == rhs)
			pair--;
		if (pair == 0)
			break;
	}
	lexer_peek_leave(lx, &s);
	if (pair > 0)
		return 0;
	if (tk != NULL)
		*tk = t;
	return 1;
}

/*
 * Consume the next balanced pair of tokens such as parenthesis or squares.
 * Returns non-zero if such tokens was found.
 */
int
lexer_if_pair(struct lexer *lx, int lhs, int rhs, struct token **tk)
{
	struct token *end;

	if (!lexer_peek_if_pair(lx, lhs, rhs, &end))
		return 0;

	lx->lx_st.st_tk = end;
	if (tk != NULL)
		*tk = end;
	return 1;
}

/*
 * Peek at the prefixes of the next token without consuming it. Returns non-zero
 * if any prefix has the given flags.
 */
int
lexer_peek_if_prefix_flags(struct lexer *lx, unsigned int flags,
    struct token **tk)
{
	struct token *px, *t;

	/* Cannot use lexer_peek() as it would move past cpp branches. */
	if (!lexer_back(lx, &t))
		return 0;
	t = token_next(t);
	if (t == NULL)
		return 0;
	TAILQ_FOREACH(px, &t->tk_prefixes, tk_entry) {
		if (px->tk_flags & flags) {
			if (tk != NULL)
				*tk = px;
			return 1;
		}
	}
	return 0;
}

/*
 * Peek until the given token type is encountered. Returns non-zero if such
 * token was found.
 */
int
lexer_peek_until(struct lexer *lx, int type, struct token **tk)
{
	struct lexer_state s;
	int peek;

	lexer_peek_enter(lx, &s);
	peek = lexer_until(lx, type, tk);
	lexer_peek_leave(lx, &s);
	return peek;
}

/*
 * Peek until the given token type is encountered and it is not nested under any
 * pairs of parenthesis nor braces but halt while trying to move beyond the
 * given stop token. Returns non-zero if such token was found.
 *
 * Assuming tk is not NULL and the stop is reached, tk will point to the stop
 * token.
 */
int
lexer_peek_until_freestanding(struct lexer *lx, int type,
    const struct token *stop, struct token **tk)
{
	struct lexer_state s;
	struct token *t = NULL;
	int nest = 0;
	int peek = 0;

	lexer_peek_enter(lx, &s);
	for (;;) {
		if (!lexer_pop(lx, &t) || t == stop || t->tk_type == LEXER_EOF)
			break;
		if (t->tk_type == TOKEN_LPAREN || t->tk_type == TOKEN_LBRACE)
			nest++;
		else if (t->tk_type == TOKEN_RPAREN ||
		    t->tk_type == TOKEN_RBRACE)
			nest--;
		if (t->tk_type == type && !nest) {
			peek = 1;
			break;
		}
	}
	lexer_peek_leave(lx, &s);
	if (tk != NULL)
		*tk = peek ? t : (struct token *)stop;
	return peek;
}

int
lexer_peek_until_comma(struct lexer *lx, const struct token *stop,
    struct token **tk)
{
	return lexer_peek_until_freestanding(lx, TOKEN_COMMA, stop, tk);
}

/*
 * Consume token(s) until the given token type is encountered. Returns non-zero
 * if such token is found.
 */
int
lexer_until(struct lexer *lx, int type, struct token **tk)
{
	for (;;) {
		struct token *t = NULL;

		if (!lexer_pop(lx, &t) || t->tk_type == LEXER_EOF)
			return 0;
		if (t->tk_type == type) {
			if (tk != NULL)
				*tk = t;
			return 1;
		}
	}
	return 0;
}

const struct diffchunk *
lexer_get_diffchunk(const struct lexer *lx, unsigned int lno)
{
	if (lx->lx_diff == NULL)
		return NULL;
	return diff_get_chunk(lx->lx_diff, lno);
}

/*
 * Looks unused but only used while debugging and therefore not declared static.
 */
void
lexer_dump(struct lexer *lx)
{
	struct token *tk;
	unsigned int i = 0;

	TAILQ_FOREACH(tk, &lx->lx_tokens, tk_entry) {
		struct token *prefix, *suffix;
		const char *str;
		int nfixes = 0;

		i++;

		TAILQ_FOREACH(prefix, &tk->tk_prefixes, tk_entry) {
			str = lexer_serialize(lx, prefix);
			fprintf(stderr, "[L] %-6u   prefix %s", i, str);

			if (prefix->tk_branch.br_pv != NULL) {
				str = lexer_serialize(lx,
				    prefix->tk_branch.br_pv);
				fprintf(stderr, ", pv %s", str);
			}
			if (prefix->tk_branch.br_nx != NULL) {
				str = lexer_serialize(lx,
				    prefix->tk_branch.br_nx);
				fprintf(stderr, ", nx %s", str);
			}
			fprintf(stderr, "\n");
			nfixes++;
		}

		str = lexer_serialize(lx, tk);
		fprintf(stderr, "[L] %-6u %s\n", i, str);

		TAILQ_FOREACH(suffix, &tk->tk_suffixes, tk_entry) {
			str = lexer_serialize(lx, suffix);
			fprintf(stderr, "[L] %-6u   suffix %s\n", i, str);
			nfixes++;
		}

		if (nfixes > 0)
			fprintf(stderr, "[L] -\n");
	}
}

/*
 * Consume empty line(s) until a line beginning with optional whitespace
 * followed by none whitespace is found. The lexer will be positioned before the
 * first none whitespace character and the given state at the beginning of the
 * same line line, thus including the whitespace if present.
 */
void
lexer_eat_lines_and_spaces(struct lexer *lx, struct lexer_state *st)
{
	int gotspaces = 0;

	if (st != NULL)
		*st = lx->lx_st;

	for (;;) {
		if (lexer_eat_lines(lx, 0, NULL)) {
			if (st != NULL)
				*st = lx->lx_st;
			gotspaces = 0;
		} else if (gotspaces) {
			break;
		}

		if (lexer_eat_spaces(lx, NULL))
			gotspaces = 1;
		else
			break;
	}
}

int
lexer_eat_lines(struct lexer *lx, int threshold, struct token **tk)
{
	struct lexer_state oldst, st;
	int nlines = 0;
	unsigned char ch;

	oldst = st = lx->lx_st;

	for (;;) {
		if (lexer_getc(lx, &ch))
			break;
		if (ch == '\r') {
			continue;
		} else if (ch == '\n') {
			oldst = lx->lx_st;
			if (++nlines == threshold)
				break;
		} else if (ch != ' ' && ch != '\t') {
			lexer_ungetc(lx);
			break;
		}
	}
	lx->lx_st = oldst;
	if (nlines == 0 || nlines < threshold || lexer_eof(lx))
		return 0;
	if (tk != NULL) {
		*tk = lexer_emit(lx, &st, &(struct token){
		    .tk_type	= TOKEN_SPACE,
		    .tk_str	= "\n",
		    .tk_len	= 1,
		});
	}
	return nlines;
}

int
lexer_eat_spaces(struct lexer *lx, struct token **tk)
{
	struct lexer_state st;
	unsigned char ch;

	st = lx->lx_st;

	do {
		if (lexer_getc(lx, &ch))
			return 0;
	} while (ch == ' ' || ch == '\t');
	lexer_ungetc(lx);

	if (st.st_off == lx->lx_st.st_off)
		return 0;
	if (tk != NULL) {
		*tk = lexer_emit(lx, &st, &(struct token){
		    .tk_type	= TOKEN_SPACE,
		});
	}
	return 1;
}

int
lexer_eof(const struct lexer *lx)
{
	return lx->lx_st.st_off == lx->lx_bf->bf_len;
}

static void
lexer_line_alloc(struct lexer *lx, unsigned int lno)
{
	unsigned int *dst;

	if (lx->lx_diff == NULL)
		return;

	/* We could end up here again after lexer_ungetc(). */
	if (lno - 1 < VECTOR_LENGTH(lx->lx_lines))
		return;

	dst = VECTOR_ALLOC(lx->lx_lines);
	if (dst == NULL)
		err(1, NULL);
	*dst = lx->lx_st.st_off;
}

int
lexer_buffer_streq(const struct lexer *lx, const struct lexer_state *st,
    const char *str)
{
	const char *buf;
	size_t buflen, len;

	buflen = lx->lx_st.st_off - st->st_off;
	if (buflen == 0)
		return 0;
	len = strlen(str);
	if (len > buflen)
		return 0;
	buf = &lx->lx_bf->bf_ptr[st->st_off];
	return strncmp(buf, str, len) == 0;
}

const char *
lexer_buffer_slice(const struct lexer *lx, const struct lexer_state *st,
    size_t *len)
{
	*len = lx->lx_st.st_off - st->st_off;
	return &lx->lx_bf->bf_ptr[st->st_off];
}

static void
lexer_expect_error(struct lexer *lx, int type, const struct token *tk,
    const char *fun, int lno)
{
	struct buffer *bf;
	struct token *t;

	/* Be quiet while about to branch. */
	if (lexer_back(lx, &t) && token_is_branch(t)) {
		lexer_trace(lx, "%s:%d: suppressed, expected %s", fun, lno,
		    lexer_serialize(lx, &(struct token){.tk_type = type}));
		return;
	}

	/* Be quiet if an error already has been emitted. */
	if (lx->lx_st.st_err++ > 0)
		return;

	/* Be quiet while peeking. */
	if (lx->lx_peek > 0)
		return;

	bf = error_begin(lx->lx_er);
	buffer_printf(bf, "%s: ", lx->lx_path);
	if (trace(lx->lx_op, 'l'))
		buffer_printf(bf, "%s:%d: ", fun, lno);
	buffer_printf(bf, "expected type %s got %s\n",
	    lexer_serialize(lx, &(struct token){.tk_type = type}),
	    lexer_serialize(lx, tk));
	error_end(lx->lx_er);
}

/*
 * Fold tokens covered by the branch into a prefix.
 */
static void
lexer_branch_fold(struct lexer *lx, struct token *src)
{
	struct token *dst, *prefix, *pv, *rm;
	size_t len, off;
	int unmute = 0;

	/* Grab a reference since the branch is about to be removed. */
	dst = src->tk_branch.br_nx;
	token_ref(dst);

	off = src->tk_off;
	len = (dst->tk_off + dst->tk_len) - off;

	prefix = token_alloc(&(struct token){
	    .tk_type	= TOKEN_CPP,
	    .tk_flags	= TOKEN_FLAG_CPP,
	});
	prefix->tk_lno = src->tk_lno;
	prefix->tk_cno = src->tk_cno;
	prefix->tk_off = off;
	prefix->tk_str = &lx->lx_bf->bf_ptr[off];
	prefix->tk_len = len;

	/*
	 * Remove all prefixes hanging of the destination covered by the new
	 * prefix token.
	 */
	while (!TAILQ_EMPTY(&dst->tk_token->tk_prefixes)) {
		struct token *pr;

		pr = TAILQ_FIRST(&dst->tk_token->tk_prefixes);
		lexer_trace(lx, "removing prefix %s", lexer_serialize(lx, pr));
		TAILQ_REMOVE(&dst->tk_token->tk_prefixes, pr, tk_entry);
		/* Completely unlink any branch. */
		while (token_branch_unlink(pr) == 0)
			continue;
		token_rele(pr);
		if (pr == dst)
			break;
	}

	lexer_trace(lx, "add prefix %s to %s",
	    lexer_serialize(lx, prefix),
	    lexer_serialize(lx, dst->tk_token));
	TAILQ_INSERT_HEAD(&dst->tk_token->tk_prefixes, prefix, tk_entry);

	/*
	 * Keep any existing prefix not covered by the new prefix token
	 * by moving them to the destination.
	 */
	pv = token_prev(src);
	for (;;) {
		struct token *tmp;

		if (pv == NULL)
			break;

		lexer_trace(lx, "keeping prefix %s", lexer_serialize(lx, pv));
		tmp = token_prev(pv);
		token_move_prefix(pv, src->tk_token, dst->tk_token);
		pv = tmp;
	}

	/*
	 * Remove all tokens up to the destination covered by the new prefix
	 * token.
	 */
	rm = src->tk_token;
	for (;;) {
		struct token *nx;

		if (rm == dst->tk_token)
			break;

		nx = token_next(rm);
		lexer_trace(lx, "removing %s", lexer_serialize(lx, rm));
		if (rm->tk_flags & TOKEN_FLAG_UNMUTE)
			unmute = 1;
		lexer_remove(lx, rm, 0);
		rm = nx;
	}

	/*
	 * If the unmute token ended up being deleted, tell doc_token() that
	 * crossing the end of this branch must cause tokens to be emitted
	 * again.
	 */
	if (unmute)
		lexer_branch_unmute(lx, dst->tk_token);

	token_rele(dst);
}

static void
lexer_branch_unmute(struct lexer *lx, struct token *tk)
{
	assert(lx->lx_unmute == NULL);
	tk->tk_flags |= TOKEN_FLAG_UNMUTE;
	token_ref(tk);
	lx->lx_unmute = tk;
}

/*
 * Find the best suited branch to fold relative to the given token while trying
 * to recover after encountering invalid source code. We do not want to fold a
 * partially consumed branch as lexer_branch() already has removed tokens making
 * it impossible to traverse the same source code again since it is no longer
 * intact. However, when reaching EOF we try to fold even partially consumed
 * branches.
 */
static struct token *
lexer_recover_branch(struct token *tk)
{
	unsigned int flags[] = {
		LEXER_BRANCH_BACKWARD | LEXER_BRANCH_INTACT,
		LEXER_BRANCH_FORWARD | LEXER_BRANCH_INTACT,
		LEXER_BRANCH_BACKWARD,
		LEXER_BRANCH_FORWARD,
	};
	size_t nflags = sizeof(flags) / sizeof(flags[0]);
	size_t i;

	for (i = 0; i < nflags; i++) {
		struct token *br;

		br = lexer_recover_branch1(tk, flags[i]);
		if (br != NULL)
			return br;
	}
	return NULL;
}

static struct token *
lexer_recover_branch1(struct token *tk, unsigned int flags)
{
	for (;;) {
		struct token *prefix;

		TAILQ_FOREACH(prefix, &tk->tk_prefixes, tk_entry) {
			if (prefix->tk_type == TOKEN_CPP_IF)
				return prefix;
			if (prefix->tk_type == TOKEN_CPP_ENDIF) {
				struct token *pv = prefix->tk_branch.br_pv;

				if ((flags & LEXER_BRANCH_INTACT) &&
				    pv->tk_type == TOKEN_CPP_ELSE &&
				    pv->tk_branch.br_pv == NULL)
					return NULL;
				return pv;
			}
		}

		if (flags & LEXER_BRANCH_FORWARD)
			tk = token_next(tk);
		else if (flags & LEXER_BRANCH_BACKWARD)
			tk = token_prev(tk);
		if (tk == NULL)
			break;
	}

	return NULL;
}

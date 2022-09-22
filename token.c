#include "token.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "util.h"

static const char	*strtoken(int);

static const struct token tkline = {
	.tk_type	= TOKEN_SPACE,
	.tk_str		= "\n",
	.tk_len		= 1,
	.tk_flags	= TOKEN_FLAG_DANGLING,
};

struct token *
token_alloc(const struct token *def)
{
	struct token *tk;

	tk = calloc(1, sizeof(*tk));
	if (tk == NULL)
		err(1, NULL);
	if (def != NULL)
		*tk = *def;
	tk->tk_refs = 1;
	TAILQ_INIT(&tk->tk_prefixes);
	TAILQ_INIT(&tk->tk_suffixes);
	return tk;
}

void
token_ref(struct token *tk)
{
	tk->tk_refs++;
}

void
token_rele(struct token *tk)
{
	assert(tk->tk_refs > 0);
	if (--tk->tk_refs > 0)
		return;

	if ((tk->tk_flags & TOKEN_FLAG_DANGLING) == 0) {
		struct token *fix;

		while ((fix = TAILQ_FIRST(&tk->tk_prefixes)) != NULL) {
			token_branch_unlink(fix);
			token_remove(&tk->tk_prefixes, fix);
		}
		while ((fix = TAILQ_FIRST(&tk->tk_suffixes)) != NULL)
			token_remove(&tk->tk_suffixes, fix);
	}
	if (tk->tk_flags & TOKEN_FLAG_DIRTY)
		free((void *)tk->tk_str);
	free(tk);
}

void
token_add_optline(struct token *tk)
{
	struct token *suffix;

	suffix = token_alloc(&tkline);
	suffix->tk_flags |= TOKEN_FLAG_OPTLINE;
	TAILQ_INSERT_TAIL(&tk->tk_suffixes, suffix, tk_entry);
}

/*
 * Remove all space suffixes from the given token. Returns the number of removed
 * suffixes.
 */
int
token_trim(struct token *tk)
{
	struct token *suffix, *tmp;
	int ntrim = 0;

	TAILQ_FOREACH_SAFE(suffix, &tk->tk_suffixes, tk_entry, tmp) {
		/*
		 * Optional spaces are never emitted and must therefore be
		 * preserved.
		 */
		if (suffix->tk_flags & TOKEN_FLAG_OPTSPACE)
			continue;

		if (suffix->tk_type == TOKEN_SPACE) {
			token_remove(&tk->tk_suffixes, suffix);
			ntrim++;
		}
	}

	return ntrim;
}

char *
token_sprintf(const struct token *tk)
{
	char *buf = NULL;
	char *val;
	const char *type;
	ssize_t bufsiz = 0;
	int i;

	type = strtoken(tk->tk_type);

	if (tk->tk_str == NULL) {
		buf = strdup(type);
		if (buf == NULL)
			err(1, NULL);
		return buf;
	}

	val = strnice(tk->tk_str, tk->tk_len);
	for (i = 0; i < 2; i++) {
		int n;

		n = snprintf(buf, bufsiz, "%s<%u:%u>(\"%s\")",
		    type, tk->tk_lno, tk->tk_cno, val);
		if (n < 0 || (buf != NULL && n >= bufsiz))
			errc(1, ENAMETOOLONG, "snprintf");
		if (buf == NULL) {
			bufsiz = n + 1;
			buf = malloc(bufsiz);
			if (buf == NULL)
				err(1, NULL);
		}
	}
	free(val);
	return buf;
}

int
token_cmp(const struct token *t1, const struct token *t2)
{
	if (t1->tk_lno < t2->tk_lno)
		return -1;
	if (t1->tk_lno > t2->tk_lno)
		return 1;
	/* Intentionally not comparing the column. */
	return 0;
}

/*
 * Returns non-zero if the given token is preceded with whitespace.
 * Such whitespace is never emitted by the lexer we therefore have to resort to
 * inspecting the source code through the underlying lexer buffer.
 */
int
token_has_indent(const struct token *tk)
{
	return tk->tk_off > 0 &&
	    (tk->tk_str[-1] == ' ' || tk->tk_str[-1] == '\t');
}

int
token_has_prefix(const struct token *tk, int type)
{
	struct token_list *list = (struct token_list *)&tk->tk_prefixes;

	return token_list_find(list, type) != NULL;
}

/*
 * Returns non-zero if the given token has at least nlines number of trailing
 * hard line(s).
 */
int
token_has_line(const struct token *tk, int nlines)
{
	const struct token *suffix;
	unsigned int flags = TOKEN_FLAG_OPTSPACE;

	assert(nlines > 0 && nlines <= 2);

	if (nlines > 1)
		flags |= TOKEN_FLAG_OPTLINE;

	TAILQ_FOREACH(suffix, &tk->tk_suffixes, tk_entry) {
		if (suffix->tk_type == TOKEN_SPACE &&
		    (suffix->tk_flags & flags) == 0)
			return 1;
	}
	return 0;
}

/*
 * Returns non-zero if the given token has trailing tabs.
 */
int
token_has_tabs(const struct token *tk)
{
	const struct token *suffix;

	TAILQ_FOREACH(suffix, &tk->tk_suffixes, tk_entry) {
		if (suffix->tk_type == TOKEN_SPACE &&
		    (suffix->tk_flags & TOKEN_FLAG_OPTSPACE) &&
		    suffix->tk_str[0] == '\t')
			return 1;
	}
	return 0;
}

/*
 * Returns non-zero if the given token has trailing spaces, including tabs.
 */
int
token_has_spaces(const struct token *tk)
{
	const struct token *suffix;

	TAILQ_FOREACH(suffix, &tk->tk_suffixes, tk_entry) {
		if (suffix->tk_type == TOKEN_SPACE &&
		    (suffix->tk_flags & TOKEN_FLAG_OPTSPACE))
			return 1;
	}
	return 0;
}

/*
 * Returns non-zero if given token has a branch continuation associated with it.
 */
int
token_is_branch(const struct token *tk)
{
	return token_get_branch((struct token *)tk) != NULL;
}

/*
 * Returns non-zero if the given token represents a declaration of the given
 * type.
 */
int
token_is_decl(const struct token *tk, int type)
{
	const struct token *nx;

	nx = token_next(tk);
	if (nx == NULL || nx->tk_type != TOKEN_LBRACE)
		return 0;

	if (tk->tk_type == TOKEN_IDENT) {
		tk = token_prev(tk);
		if (tk == NULL)
			return 0;
	}
	return tk->tk_type == type;
}

/*
 * Returns non-zero if the given token can be moved.
 */
int
token_is_moveable(const struct token *tk)
{
	struct token_list *suffixes = (struct token_list *)&tk->tk_suffixes;
	const struct token *prefix;

	TAILQ_FOREACH(prefix, &tk->tk_prefixes, tk_entry) {
		if (prefix->tk_type == TOKEN_COMMENT ||
		    (prefix->tk_flags & TOKEN_FLAG_CPP))
			return 0;
	}

	if (token_list_find(suffixes, TOKEN_COMMENT) != NULL)
		return 0;

	return 1;
}

/*
 * Returns the branch continuation associated with the given token if present.
 */
struct token *
token_get_branch(struct token *tk)
{
	struct token *br;

	br = token_list_find(&tk->tk_prefixes, TOKEN_CPP_ELSE);
	if (br == NULL)
		return NULL;
	return br->tk_branch.br_pv;
}

struct token *
token_next0(struct token *tk)
{
	return TAILQ_NEXT(tk, tk_entry);
}

struct token *
token_prev0(struct token *tk)
{
	return TAILQ_PREV(tk, token_list, tk_entry);
}

void
token_remove(struct token_list *tl, struct token *tk)
{
	TAILQ_REMOVE(tl, tk, tk_entry);
	token_rele(tk);
}

void
token_list_copy(struct token_list *src, struct token_list *dst)
{
	struct token *tk;

	TAILQ_FOREACH(tk, src, tk_entry) {
		struct token *cp;

		cp = token_alloc(tk);
		TAILQ_INSERT_TAIL(dst, cp, tk_entry);
	}
}

void
token_list_move(struct token_list *src, struct token_list *dst)
{
	struct token *tk;

	while ((tk = TAILQ_FIRST(src)) != NULL) {
		assert((tk->tk_flags & TOKEN_FLAG_CPP) == 0);
		TAILQ_REMOVE(src, tk, tk_entry);
		TAILQ_INSERT_TAIL(dst, tk, tk_entry);
	}
}

struct token *
token_list_find(struct token_list *list, int type)
{
	struct token *tk;

	TAILQ_FOREACH(tk, list, tk_entry) {
		if (tk->tk_type == type)
			return tk;
	}
	return NULL;
}

void
token_move_suffixes(struct token *src, struct token *dst, int type)
{
	struct token *suffix, *tmp;

	TAILQ_FOREACH_SAFE(suffix, &src->tk_suffixes, tk_entry, tmp) {
		if (suffix->tk_type != type)
			continue;

		TAILQ_REMOVE(&src->tk_suffixes, suffix, tk_entry);
		TAILQ_INSERT_TAIL(&dst->tk_suffixes, suffix, tk_entry);
	}
}

/*
 * Unlink any branch associated with the given token. Returns one of the
 * following:
 *
 *     1    Branch completely unlinked.
 *     0    Branch not completely unlinked.
 *     -1   Not a branch token.
 */
int
token_branch_unlink(struct token *tk)
{
	struct token *nx, *pv;

	pv = tk->tk_branch.br_pv;
	nx = tk->tk_branch.br_nx;

	if (tk->tk_type == TOKEN_CPP_IF) {
		if (nx != NULL)
			token_branch_unlink(nx);
		/* Branch exhausted. */
		tk->tk_type = TOKEN_CPP;
		return 1;
	} else if (tk->tk_type == TOKEN_CPP_ELSE ||
	    tk->tk_type == TOKEN_CPP_ENDIF) {
		if (pv != NULL) {
			pv->tk_branch.br_nx = NULL;
			tk->tk_branch.br_pv = NULL;
			if (pv->tk_type == TOKEN_CPP_IF)
				token_branch_unlink(pv);
			pv = NULL;
		} else if (nx != NULL) {
			nx->tk_branch.br_pv = NULL;
			tk->tk_branch.br_nx = NULL;
			if (nx->tk_type == TOKEN_CPP_ENDIF)
				token_branch_unlink(nx);
			nx = NULL;
		}
		if (pv == NULL && nx == NULL) {
			/* Branch exhausted. */
			tk->tk_type = TOKEN_CPP;
			return 1;
		}
		return 0;
	}
	return -1;
}

static const char *
strtoken(int type)
{
	switch (type) {
#define T(t, s, f) case t: return &#t[sizeof("TOKEN_") - 1];
#define S(t, s, f) T(t, s, f)
#include "token-defs.h"
	}
	if (type == LEXER_EOF)
		return "EOF";
	return NULL;
}
#include "style.h"

#include "config.h"

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer.h"
#include "cdefs.h"
#include "error.h"
#include "lexer.h"
#include "options.h"
#include "token.h"
#include "util.h"

#ifdef HAVE_UTHASH
#  include <uthash.h>
#else
#  include "compat-uthash.h"
#endif

/*
 * Return values for yaml parser routines. Only one of the following may be
 * returned but disjoint values are favored allowing the caller to catch
 * multiple return values.
 */
#define NONE	0x00000000u
#define GOOD	0x00000001u
#define SKIP	0x00000002u
#define FAIL	0x00000004u

/* Continuation of token types used to represent YAML primitives. */
enum yaml_type {
	DocumentBegin = Last + 1,
	DocumentEnd,
	Colon,
	Sequence,
	String,
	Integer,
	Unknown,
};

struct style {
	const struct options	*st_op;
	int			 st_scope;
	unsigned int		 st_options[Last];
};

struct style_option {
	UT_hash_handle	 hh;
	int		 so_scope;
	const char	*so_key;
	int		 so_type;
	int		 (*so_parse)(struct style *, struct lexer *,
	    const struct style_option *);
	int		 so_val[32];
};

static void	style_defaults(struct style *);
static int	style_parse_yaml(struct style *, const char *,
    const struct buffer *, const struct options *);
static int	style_parse_yaml1(struct style *, struct lexer *);

static struct token	*yaml_read(struct lexer *, void *);
static char		*yaml_serialize(const struct token *);
static struct token	*yaml_keyword(struct lexer *,
    const struct lexer_state *);

static int	parse_bool(struct style *, struct lexer *,
    const struct style_option *);
static int	parse_enum(struct style *, struct lexer *,
    const struct style_option *);
static int	parse_integer(struct style *, struct lexer *,
    const struct style_option *);
static int	parse_string(struct style *, struct lexer *,
    const struct style_option *);
static int	parse_nested(struct style *, struct lexer *,
    const struct style_option *);
static int	parse_AlignOperands(struct style *, struct lexer *,
    const struct style_option *);
static int	parse_IncludeCategories(struct style *, struct lexer *,
    const struct style_option *);

static const char	*stryaml(enum yaml_type);

static struct style_option *keywords = NULL;

void
style_init(void)
{
	static struct style_option kw[] = {
#define S(t)	{0},	0,	#t,	(t)
#define N(s, t)	{0},	(s),	#t,	(t)
#define E(t)	{0},	0,	#t,	(t),	NULL,	{0}
#define P(t, s)	{0},	0,	(s),	(t),	NULL,	{0}

		{ S(AlignAfterOpenBracket), parse_enum,
		  { Align, DontAlign, AlwaysBreak, BlockIndent } },

		{ S(AlignEscapedNewlines), parse_enum,
		  { DontAlign, Left, Right } },

		{ S(AlignOperands), parse_AlignOperands,
		  { Align, DontAlign, AlignAfterOperator, True, False } },

		{ S(AlwaysBreakAfterReturnType), parse_enum,
		  { None, All, TopLevel, AllDefinitions, TopLevelDefinitions } },

		{ S(BraceWrapping), parse_nested, {0} },
		{ N(BraceWrapping, AfterCaseLabel), parse_bool, {0} },
		{ N(BraceWrapping, AfterClass), parse_bool, {0} },
		{ N(BraceWrapping, AfterControlStatement), parse_enum,
		  { Never, MultiLine, Always, True, False } },
		{ N(BraceWrapping, AfterEnum), parse_bool, {0} },
		{ N(BraceWrapping, AfterFunction), parse_bool, {0} },
		{ N(BraceWrapping, AfterNamespace), parse_bool, {0} },
		{ N(BraceWrapping, AfterObjCDeclaration), parse_bool, {0} },
		{ N(BraceWrapping, AfterStruct), parse_bool, {0} },
		{ N(BraceWrapping, AfterUnion), parse_bool, {0} },
		{ N(BraceWrapping, AfterExternBlock), parse_bool, {0} },
		{ N(BraceWrapping, BeforeCatch), parse_bool, {0} },
		{ N(BraceWrapping, BeforeElse), parse_bool, {0} },
		{ N(BraceWrapping, BeforeLambdaBody), parse_bool, {0} },
		{ N(BraceWrapping, BeforeWhile), parse_bool, {0} },
		{ N(BraceWrapping, IndentBraces), parse_bool, {0} },
		{ N(BraceWrapping, SplitEmptyFunction), parse_bool, {0} },
		{ N(BraceWrapping, SplitEmptyRecord), parse_bool, {0} },
		{ N(BraceWrapping, SplitEmptyNamespace), parse_bool, {0} },

		{ S(BreakBeforeBinaryOperators), parse_enum,
		  { None, NonAssignment, All } },

		{ S(BreakBeforeBraces), parse_enum,
		  { Attach, Linux, Mozilla, Stroustrup, Allman, Whitesmiths,
		    GNU, WebKit, Custom } },

		{ S(BreakBeforeTernaryOperators), parse_bool, {0} },

		{ S(ColumnLimit), parse_integer, {0} },

		{ S(ContinuationIndentWidth), parse_integer, {0} },

		{ S(IncludeCategories), parse_IncludeCategories, {0} },
		{ N(IncludeCategories, CaseSensitive), parse_bool, {0} },
		{ N(IncludeCategories, Priority), parse_integer, {0} },
		{ N(IncludeCategories, Regex), parse_string, {0} },
		{ N(IncludeCategories, SortPriority), parse_integer, {0} },

		{ S(IndentWidth), parse_integer, {0} },

		{ S(UseTab), parse_enum,
		  { Never, ForIndentation, ForContinuationAndIndentation,
		    AlignWithSpaces, Always } },

		/* enum */
		{ E(Align) },
		{ E(AlignAfterOperator) },
		{ E(AlignWithSpaces) },
		{ E(All) },
		{ E(AllDefinitions) },
		{ E(Allman) },
		{ E(Always) },
		{ E(AlwaysBreak) },
		{ E(Attach) },
		{ E(BlockIndent) },
		{ E(Custom) },
		{ E(DontAlign) },
		{ E(ForContinuationAndIndentation) },
		{ E(ForIndentation) },
		{ E(GNU) },
		{ E(Left) },
		{ E(Linux) },
		{ E(Mozilla) },
		{ E(Never) },
		{ E(NonAssignment) },
		{ E(None) },
		{ E(Right) },
		{ E(Stroustrup) },
		{ E(TopLevel) },
		{ E(TopLevelDefinitions) },
		{ E(WebKit) },
		{ E(Whitesmiths) },
		{ E(MultiLine) },

		/* primitives */
		{ P(Colon, ":") },
		{ P(DocumentBegin, "---"), },
		{ P(DocumentEnd, "..."), },
		{ P(False, "false") },
		{ P(Integer, "Integer") },
		{ P(Sequence, "-") },
		{ P(True, "true") },

#undef Y
#undef S
	};
	size_t nkeywords = sizeof(kw) / sizeof(kw[0]);
	size_t i;

	for (i = 0; i < nkeywords; i++)
		HASH_ADD_STR(keywords, so_key, &kw[i]);
}

void
style_teardown(void)
{
	HASH_CLEAR(hh, keywords);
}

struct style *
style_parse(const char *path, const struct options *op)
{
	struct buffer *bf = NULL;
	struct style *st;
	int error = 0;

	st = calloc(1, sizeof(*st));
	if (st == NULL)
		err(1, NULL);
	st->st_op = op;
	style_defaults(st);

	if (path != NULL) {
		bf = buffer_read(path);
	} else {
		int fd;

		path = ".clang-format";
		fd = searchpath(path, NULL);
		if (fd != -1) {
			bf = buffer_read_fd(fd);
			close(fd);
		}
	}
	if (bf != NULL)
		error = style_parse_yaml(st, path, bf, op);
	buffer_free(bf);
	if (error) {
		style_free(st);
		st = NULL;
	}
	return st;
}

void
style_free(struct style *st)
{
	if (st == NULL)
		return;
	free(st);
}

unsigned int
style(const struct style *st, int option)
{
	assert(option < Last);
	return st->st_options[option];
}

int
style_align(const struct style *st)
{
	return style(st, AlignAfterOpenBracket) == Align ||
	    style(st, AlignOperands) == Align;
}

int
style_brace_wrapping(const struct style *st, int option)
{
	switch (st->st_options[BreakBeforeBraces]) {
	case Linux:
		switch (option) {
		case AfterFunction:
			return 1;
		}
		break;
	}
	return st->st_options[option] == True;
}

static void
style_defaults(struct style *st)
{
	st->st_options[AlignAfterOpenBracket] = DontAlign;
	st->st_options[AlignEscapedNewlines] = Right;
	st->st_options[AlignOperands] = DontAlign;
	st->st_options[AlwaysBreakAfterReturnType] = AllDefinitions;
	st->st_options[BreakBeforeBinaryOperators] = None;
	st->st_options[BreakBeforeBraces] = Linux;
	st->st_options[BreakBeforeTernaryOperators] = False;
	st->st_options[ColumnLimit] = 80;
	st->st_options[ContinuationIndentWidth] = 4;
	st->st_options[IndentWidth] = 8;
	st->st_options[UseTab] = Always;
}

static int
style_parse_yaml(struct style *st, const char *path, const struct buffer *bf,
    const struct options *op)
{
	struct error *er;
	struct lexer *lx;
	int error = 0;

	er = error_alloc(1);
	lx = lexer_alloc(&(const struct lexer_arg){
	    .path	= path,
	    .bf		= bf,
	    .er		= er,
	    .diff	= NULL,
	    .op		= op,
	    .callbacks	= {
		.read		= yaml_read,
		.serialize	= yaml_serialize,
		.arg		= st
	    },
	});
	if (lx == NULL) {
		error = 1;
		goto out;
	}
	error = style_parse_yaml1(st, lx);

out:
	lexer_free(lx);
	error_free(er);
	return error;
}

static int
style_parse_yaml1(struct style *st, struct lexer *lx)
{
	for (;;) {
		const struct style_option *so;
		struct token *key;
		int error = 0;

		if (lexer_if(lx, LEXER_EOF, NULL))
			break;
		if (lexer_peek_if(lx, Sequence, NULL))
			break;

		if (lexer_if(lx, DocumentBegin, NULL))
			continue;
		if (lexer_if(lx, DocumentEnd, NULL))
			continue;

		if (!lexer_peek(lx, &key))
			break;
		so = (struct style_option *)key->tk_token;
		if (so != NULL && so->so_scope != st->st_scope)
			break;
		if (so != NULL)
			error = so->so_parse(st, lx, so);
		if (error & (GOOD | SKIP)) {
			continue;
		} else if (error & FAIL) {
			break;
		} else {
			struct token *val;

			/* Best effort, try to continue parsing. */
			lexer_pop(lx, &key);
			lexer_if(lx, Colon, NULL);
			if (lexer_peek_if(lx, Sequence, NULL)) {
				/* Ignore sequences. */
				while (lexer_if(lx, Sequence, NULL))
					lexer_pop(lx, &val);
			} else {
				lexer_pop(lx, &val);
			}
			if (trace(st->st_op, 's')) {
				lexer_error(lx, "unknown option %s",
				    lexer_serialize(lx, key));
			}
		}
	}

	return trace(st->st_op, 's') ? lexer_get_error(lx) : 0;
}

static struct token *
yaml_read(struct lexer *lx, void *UNUSED(arg))
{
	struct lexer_state s;
	struct token *tk;
	unsigned char ch;

again:
	lexer_eat_lines_and_spaces(lx, NULL);
	s = lexer_get_state(lx);
	if (lexer_getc(lx, &ch))
		goto eof;

	if (ch == '#') {
		for (;;) {
			if (lexer_getc(lx, &ch))
				goto eof;
			if (ch == '\n')
				break;
		}
		goto again;
	}

	if (isalpha(ch)) {
		do {
			if (lexer_getc(lx, &ch))
				goto eof;
		} while (isalpha(ch));
		lexer_ungetc(lx);
		return yaml_keyword(lx, &s);
	}

	if (isdigit(ch)) {
		int overflow = 0;
		int digit = 0;

		while (isdigit(ch)) {
			int x = ch - '0';

			if (digit > INT_MAX / 10)
				overflow = 1;
			else
				digit *= 10;

			if (digit > INT_MAX - x)
				overflow = 1;
			else
				digit += x;

			if (lexer_getc(lx, &ch))
				goto eof;
		}
		lexer_ungetc(lx);

		tk = lexer_emit(lx, &s, NULL);
		tk->tk_type = Integer;
		tk->tk_int = digit;
		if (overflow) {
			char *str;

			str = yaml_serialize(tk);
			lexer_error(lx, "integer %s too large", str);
			free(str);
		}
		return tk;
	}

	if (ch == '-' || ch == '.' || ch == ':') {
		unsigned char needle = ch;

		do {
			if (lexer_getc(lx, &ch))
				goto eof;
		} while (ch == needle);
		lexer_ungetc(lx);
		return yaml_keyword(lx, &s);
	}

	if (ch == '\'') {
		for (;;) {
			if (lexer_getc(lx, &ch))
				goto eof;
			if (ch == '\'')
				break;
		}
		tk = lexer_emit(lx, &s, NULL);
		tk->tk_type = String;
		return tk;
	}

	tk = lexer_emit(lx, &s, NULL);
	tk->tk_type = Unknown;
	return tk;

eof:
	tk = lexer_emit(lx, &s, NULL);
	tk->tk_type = LEXER_EOF;
	return tk;
}

static char *
yaml_serialize(const struct token *tk)
{
	char *val = NULL;
	char *buf;
	ssize_t bufsiz = 256;
	int n;

	buf = malloc(bufsiz);
	if (tk->tk_type < Last) {
		val = strnice(tk->tk_str, tk->tk_len);
		n = snprintf(buf, bufsiz, "Keyword<%u:%u>(\"%s\")",
		    tk->tk_lno, tk->tk_cno, val);
	} else {
		val = strnice(tk->tk_str, tk->tk_len);
		n = snprintf(buf, bufsiz, "%s<%u:%u>(\"%s\")",
		    stryaml(tk->tk_type), tk->tk_lno, tk->tk_cno, val);
	}
	if (n < 0)
		err(1, "asprintf");
	free(val);
	return buf;
}

static struct token *
yaml_keyword(struct lexer *lx, const struct lexer_state *st)
{
	const struct style_option *so;
	struct token *tk;

	tk = lexer_emit(lx, st, NULL);
	HASH_FIND(hh, keywords, tk->tk_str, tk->tk_len, so);
	if (so == NULL) {
		tk->tk_type = Unknown;
		return tk;
	}
	tk->tk_type = so->so_type;
	if (so->so_parse != NULL)
		tk->tk_token = (void *)so;
	return tk;
}

static int
parse_bool(struct style *st, struct lexer *lx, const struct style_option *so)
{
	struct token *key, *val;

	if (!lexer_if(lx, so->so_type, &key))
		return NONE;
	if (!lexer_expect(lx, Colon, NULL))
		return FAIL;
	if (!lexer_if(lx, True, &val) && !lexer_if(lx, False, &val)) {
		(void)lexer_pop(lx, &val);
		lexer_error(lx, "unknown value %s for option %s",
		    lexer_serialize(lx, val), lexer_serialize(lx, key));
		return SKIP;
	}
	st->st_options[key->tk_type] = val->tk_type;
	return GOOD;
}

static int
parse_enum(struct style *st, struct lexer *lx, const struct style_option *so)
{
	struct token *key, *val;
	const int *v;

	if (!lexer_if(lx, so->so_type, &key))
		return NONE;
	if (!lexer_expect(lx, Colon, NULL))
		return FAIL;

	for (v = so->so_val; *v != 0; v++) {
		if (lexer_if(lx, *v, &val)) {
			st->st_options[key->tk_type] = val->tk_type;
			return GOOD;
		}
	}

	(void)lexer_pop(lx, &val);
	lexer_error(lx, "unknown value %s for option %s",
	    lexer_serialize(lx, val), lexer_serialize(lx, key));
	return SKIP;
}

static int
parse_integer(struct style *st, struct lexer *lx, const struct style_option *so)
{
	struct token *key, *val;

	if (!lexer_if(lx, so->so_type, &key))
		return NONE;
	if (!lexer_expect(lx, Colon, NULL))
		return FAIL;
	if (!lexer_expect(lx, Integer, &val)) {
		(void)lexer_pop(lx, &val);
		lexer_error(lx, "unknown value %s for option %s",
		    lexer_serialize(lx, val), lexer_serialize(lx, key));
		return SKIP;
	}
	st->st_options[key->tk_type] = val->tk_int;
	return GOOD;
}

static int
parse_string(struct style *UNUSED(st), struct lexer *lx,
    const struct style_option *so)
{
	struct token *key, *val;

	if (!lexer_if(lx, so->so_type, &key))
		return NONE;
	if (!lexer_expect(lx, Colon, NULL))
		return FAIL;
	if (!lexer_expect(lx, String, &val)) {
		(void)lexer_pop(lx, &val);
		lexer_error(lx, "unknown value %s for option %s",
		    lexer_serialize(lx, val), lexer_serialize(lx, key));
		return SKIP;
	}
	return GOOD;
}

static int
parse_nested(struct style *st, struct lexer *lx, const struct style_option *so)
{
	int scope;

	if (!lexer_if(lx, so->so_type, NULL))
		return NONE;
	if (!lexer_expect(lx, Colon, NULL))
		return FAIL;
	scope = st->st_scope;
	st->st_scope = so->so_type;
	style_parse_yaml1(st, lx);
	st->st_scope = scope;
	return GOOD;
}

static int
parse_AlignOperands(struct style *st, struct lexer *lx,
    const struct style_option *so)
{
	int error;

	error = parse_enum(st, lx, so);
	if (error & GOOD) {
		if (st->st_options[AlignOperands] == True)
			st->st_options[AlignOperands] = Align;
		else if (st->st_options[AlignOperands] == False)
			st->st_options[AlignOperands] = DontAlign;
	}
	return error;
}

static int
parse_IncludeCategories(struct style *st, struct lexer *lx,
    const struct style_option *so)
{
	int scope;

	if (!lexer_if(lx, so->so_type, NULL))
		return NONE;
	if (!lexer_expect(lx, Colon, NULL))
		return FAIL;

	scope = st->st_scope;
	st->st_scope = so->so_type;
	while (lexer_if(lx, Sequence, NULL))
		style_parse_yaml1(st, lx);
	st->st_scope = scope;
	return GOOD;
}

static const char *
stryaml(enum yaml_type type)
{
	switch (type) {
#define CASE(t) case t: return #t; break
	CASE(DocumentBegin);
	CASE(DocumentEnd);
	CASE(Colon);
	CASE(Sequence);
	CASE(String);
	CASE(Integer);
	CASE(Unknown);
#undef CASE
	}
	if (type == LEXER_EOF)
		return "EOF";
	return NULL;
}

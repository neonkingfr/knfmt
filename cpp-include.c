#include "cpp-include.h"

#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "buffer.h"
#include "options.h"
#include "style.h"
#include "token.h"
#include "util.h"
#include "vector.h"

struct cpp_include {
	VECTOR(struct include)	 includes;
	struct token_list	*prefixes;
	struct token		*after;
};

struct include {
	struct token	*tk;
	struct {
		const char	*str;
		size_t		 len;
	} path;
};

static int	sort_includes(const struct options *, const struct style *);

static void	cpp_include_exec(struct cpp_include *);
static void	cpp_include_reset(struct cpp_include *);

static int	include_cmp(const void *, const void *);

static const char	*findpath(const char *, size_t, size_t *);

static int	prefix_has_line(const struct token *);
static void	token_add_line(struct token *);
static void	token_trim_line(struct token *);

struct cpp_include *
cpp_include_alloc(const struct options *op, const struct style *st)
{
	struct cpp_include *ci;

	if (!sort_includes(op, st))
		return NULL;

	ci = ecalloc(1, sizeof(*ci));
	if (VECTOR_INIT(ci->includes) == NULL)
		err(1, NULL);
	return ci;
}

void
cpp_include_free(struct cpp_include *ci)
{
	if (ci == NULL)
		return;

	cpp_include_reset(ci);
	VECTOR_FREE(ci->includes);
	free(ci);
}

void
cpp_include_enter(struct cpp_include *ci, struct token_list *prefixes)
{
	if (ci == NULL)
		return;

	assert(ci->prefixes == NULL);
	ci->prefixes = prefixes;
}

void
cpp_include_leave(struct cpp_include *ci)
{
	if (ci == NULL)
		return;

	cpp_include_exec(ci);
	cpp_include_reset(ci);
	ci->prefixes = NULL;
}

void
cpp_include_add(struct cpp_include *ci, struct token *tk)
{
	if (ci == NULL)
		return;

	if (tk->tk_type == TOKEN_CPP_INCLUDE) {
		struct include *include;

		if (VECTOR_EMPTY(ci->includes))
			ci->after = token_prev(tk);
		include = VECTOR_ALLOC(ci->includes);
		if (include == NULL)
			err(1, NULL);
		token_ref(tk);
		include->tk = tk;
		if (!prefix_has_line(tk))
			return;
	}

	cpp_include_exec(ci);
	cpp_include_reset(ci);
}

static int
sort_includes(const struct options *op, const struct style *st)
{
	return (op->op_flags & OPTIONS_SIMPLE) ||
	    style(st, SortIncludes) == CaseSensitive;
}

static void
cpp_include_exec(struct cpp_include *ci)
{
	struct token *after = ci->after;
	size_t i, nincludes;
	unsigned int nbrackets = 0;
	unsigned int nquotes = 0;
	unsigned int nslashes = 0;
	int doline;

	nincludes = VECTOR_LENGTH(ci->includes);
	if (nincludes < 2)
		return;

	for (i = 0; i < nincludes; i++) {
		struct include *include = &ci->includes[i];
		const struct token *tk = include->tk;
		const char *path;
		size_t len;

		path = findpath(tk->tk_str, tk->tk_len, &len);
		if (path == NULL)
			return;
		include->path.str = path;
		include->path.len = len;

		if (path[0] == '<')
			nbrackets++;
		if (path[0] == '"')
			nquotes++;
		if (memchr(path, '/', len) != NULL)
			nslashes++;
		if ((nbrackets > 0 && nquotes > 0) || nslashes > 0)
			return;
	}

	doline = prefix_has_line(VECTOR_LAST(ci->includes)->tk);

	qsort(ci->includes, VECTOR_LENGTH(ci->includes), sizeof(*ci->includes),
	    include_cmp);

	for (i = 0; i < nincludes; i++) {
		struct include *include = &ci->includes[i];

		token_trim_line(include->tk);
		if (doline && i + 1 == nincludes)
			token_add_line(include->tk);
		token_list_remove(ci->prefixes, include->tk);
		if (after != NULL) {
			token_list_insert_after(ci->prefixes, after,
			    include->tk);
		} else {
			token_list_insert(ci->prefixes, include->tk);
		}
		after = include->tk;
	}
}

static void
cpp_include_reset(struct cpp_include *ci)
{
	while (!VECTOR_EMPTY(ci->includes)) {
		struct include *include;

		include = VECTOR_POP(ci->includes);
		token_rele(include->tk);
	}
	ci->after = NULL;
}

static int
include_cmp(const void *p1, const void *p2)
{
	const struct include *a = p1;
	const struct include *b = p2;

	return token_strcmp(a->tk, b->tk);
}

static const char *
findpath(const char *str, size_t len, size_t *pathlen)
{
	const char *eo, *so;
	char c;

	so = memchr(str, '"', len);
	if (so == NULL)
		so = memchr(str, '<', len);
	if (so == NULL)
		return NULL;
	c = so[0] == '"' ? '"' : '>';
	len -= (size_t)(so - str);
	eo = memchr(&so[1], c, len);
	if (eo == NULL)
		return NULL;
	*pathlen = (size_t)(eo - so) + 1;
	return so;
}

static int
prefix_has_line(const struct token *tk)
{
	const char *str = tk->tk_str;
	size_t len = tk->tk_len;

	return len >= 2 && str[len - 1] == '\n' && str[len - 2] == '\n';
}

static void
token_add_line(struct token *tk)
{
	struct buffer *bf;

	bf = buffer_alloc(128);
	buffer_printf(bf, "%.*s\n", (int)tk->tk_len, tk->tk_str);
	tk->tk_len = bf->bf_len;
	tk->tk_str = buffer_release(bf);
	tk->tk_flags |= TOKEN_FLAG_DIRTY;
	buffer_free(bf);
}

static void
token_trim_line(struct token *tk)
{
	const char *str = tk->tk_str;
	size_t len = tk->tk_len;

	if (len > 2 && str[len - 1] == '\n' && str[len - 2] == '\n')
		tk->tk_len--;
}
#include "cpp-align.h"

#include "config.h"

#include <ctype.h>
#include <string.h>

#include "buffer.h"
#include "doc.h"
#include "options.h"
#include "ruler.h"
#include "style.h"
#include "token.h"

static const char	*nextline(const char *, size_t, const char **);

/*
 * Align of line continuations.
 */
char *
cpp_align(const struct token *tk, const struct style *st,
    const struct options *op)
{
	struct ruler rl;
	struct buffer *bf;
	struct doc *dc;
	const char *nx, *str;
	char *p;
	size_t len;
	int nlines = 0;
	int usetab = style(st, UseTab) != Never;

	str = tk->tk_str;
	len = tk->tk_len;
	if (nextline(str, len, &nx) == NULL)
		return NULL;

	bf = buffer_alloc(len);
	dc = doc_alloc(DOC_CONCAT, NULL);
	switch (style(st, AlignEscapedNewlines)) {
	case DontAlign:
		ruler_init(&rl, 1, RULER_ALIGN_FIXED);
		break;
	case Left:
		ruler_init(&rl, 0, usetab ? RULER_ALIGN_TABS : RULER_ALIGN_MIN);
		break;
	case Right:
		ruler_init(&rl, style(st, ColumnLimit) - style(st, IndentWidth),
		    RULER_ALIGN_MAX | (usetab ? RULER_ALIGN_TABS : 0));
		break;
	}

	for (;;) {
		struct doc *concat;
		const char *ep, *sp;
		size_t cpplen, linelen;

		concat = doc_alloc(DOC_CONCAT, dc);

		sp = str;
		ep = nextline(sp, len, &nx);
		if (ep == NULL)
			break;

		if (nlines > 0)
			doc_alloc(DOC_HARDLINE, concat);
		cpplen = (size_t)(ep - sp);
		if (cpplen > 0)
			doc_literal_n(sp, cpplen, concat);
		ruler_insert(&rl, tk, concat, 1,
		    doc_width(concat, bf, st, op), 0);
		doc_literal("\\", concat);

		linelen = (size_t)(nx - str);
		len -= linelen;
		str += linelen;
		nlines++;
	}
	if (len > 0) {
		if (nlines > 0)
			doc_alloc(DOC_HARDLINE, dc);
		doc_literal_n(str, len, dc);
	}

	/* Alignment only wanted for multiple lines. */
	if (nlines > 1)
		ruler_exec(&rl);
	doc_exec(dc, NULL, bf, st, op, 0);
	buffer_putc(bf, '\0');

	p = buffer_release(bf);
	ruler_free(&rl);
	doc_free(dc);
	buffer_free(bf);
	return p;
}

/*
 * Returns a pointer to the end of current line assuming it's a line
 * continuation.
 */
static const char *
nextline(const char *str, size_t len, const char **nx)
{
	const char *p;

	p = memchr(str, '\n', len);
	if (p == NULL || p == str || p[-1] != '\\')
		return NULL;
	*nx = &p[1];
	p--;	/* consume '\\' */
	while (p > str && isspace((unsigned char)p[-1]))
		p--;
	return p;
}
#include <stddef.h>	/* size_t */

struct buffer;
struct doc;
struct lexer;
struct options;
struct style;
struct token;

/* Keep in sync with DESIGN. */
enum doc_type {
	DOC_CONCAT,
	DOC_GROUP,
	DOC_INDENT,
	DOC_DEDENT,
	DOC_ALIGN,
	DOC_LITERAL,
	DOC_VERBATIM,
	DOC_LINE,
	DOC_SOFTLINE,
	DOC_HARDLINE,
	DOC_OPTLINE,
	DOC_MUTE,
	DOC_OPTIONAL,
	DOC_MINIMIZE,
	DOC_SCOPE,
};

struct doc_minimize {
	enum {
		DOC_MINIMIZE_INDENT,
	} type;

	union {
		int indent;
	};

	struct {
		unsigned int	nlines;
		unsigned int	nexceeds;
		double		sum;
	} penality;

	unsigned int flags;
/* Unconditionally favor this entry. */
#define DOC_MINIMIZE_FORCE	0x00000001u
};

struct doc_align {
	int	indent;
	int	spaces;
	int	tabalign;
};

#define DOC_EXEC_DIFF	    0x00000001u
#define DOC_EXEC_TRACE	    0x00000002u
#define DOC_EXEC_WIDTH	    0x00000004u
#define DOC_EXEC_TRIM	    0x00000008u

void		doc_exec(const struct doc *, struct lexer *, struct buffer *,
    const struct style *, const struct options *, unsigned int);
unsigned int	doc_width(const struct doc *, struct buffer *,
    const struct style *, const struct options *);
void		doc_free(struct doc *);
void		doc_append(struct doc *, struct doc *);
void		doc_append_before(struct doc *, struct doc *);
void		doc_remove(struct doc *, struct doc *);
void		doc_remove_tail(struct doc *);
void		doc_set_indent(struct doc *, int);
void		doc_set_align(struct doc *, const struct doc_align *);

#define doc_alloc(a, b) \
	doc_alloc0((a), (b), 0, __func__, __LINE__)
struct doc	*doc_alloc0(enum doc_type, struct doc *, int, const char *,
    int);

/*
 * Sentinels honored by indent allocation routines. The numbers are something
 * arbitrary large enough to never conflict with any actual indent.
 */
/* Entering parenthesis. */
#define DOC_INDENT_PARENS	0x40000000
/* Force indentation. */
#define DOC_INDENT_FORCE	0x20000000
/*
 * Only emit indentation if at least one hard line has been emitted within the
 * current scope.
 */
#define DOC_INDENT_NEWLINE	0x10000000

#define doc_alloc_indent(a, b) \
	doc_alloc_indent0(DOC_INDENT, (a), (b), __func__, __LINE__)
#define doc_alloc_dedent(a) \
	doc_alloc_indent0(DOC_DEDENT, 0, (a), __func__, __LINE__)
struct doc	*doc_alloc_indent0(enum doc_type, int, struct doc *,
    const char *, int);

#define doc_minimize(a, b) \
    doc_minimize0(a, b, sizeof(b)/sizeof((b)[0]), __func__, __LINE__)
struct doc	*doc_minimize0(struct doc *, const struct doc_minimize *,
    size_t, const char *, int);

#define doc_literal(a, b) \
	doc_literal0((a), 0, (b), __func__, __LINE__)
#define doc_literal_n(a, b, c) \
	doc_literal0((a), (b), (c), __func__, __LINE__)
struct doc	*doc_literal0(const char *, size_t, struct doc *,
    const char *, int);

#define doc_token(a, b) \
	doc_token0((a), (b), DOC_LITERAL, __func__, __LINE__)
struct doc	*doc_token0(const struct token *, struct doc *, enum doc_type,
    const char *, int);

int	doc_max(const struct doc *);

void	doc_annotate(struct doc *, const char *);

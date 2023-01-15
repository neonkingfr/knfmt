#include "parser-func.h"

#include "doc.h"
#include "lexer.h"
#include "parser-attributes.h"
#include "parser-decl.h"
#include "parser-priv.h"
#include "parser-type.h"
#include "parser-stmt.h"
#include "ruler.h"
#include "style.h"
#include "token.h"

struct parser_func_proto_arg {
	struct doc		*dc;
	struct ruler		*rl;
	const struct token	*type;
	unsigned int		 flags;
#define PARSER_EXEC_FUNC_PROTO_IMPL		0x00000001u
};

static int	parser_func_impl1(struct parser *, struct doc *,
    struct ruler *, const struct token *);
static int	parser_func_proto(struct parser *, struct doc **,
    struct parser_func_proto_arg *);

static int	peek_impl_line(struct parser *);

/*
 * Returns non-zero if the next tokens denotes a function. The type argument
 * points to the last token of the return type.
 */
enum parser_func_peek
parser_func_peek(struct parser *pr, struct token **type)
{
	struct lexer_state s;
	struct lexer *lx = pr->pr_lx;
	enum parser_func_peek peek = PARSER_FUNC_PEEK_NONE;

	lexer_peek_enter(lx, &s);
	if (parser_type_peek(pr, type, 0) &&
	    lexer_seek(lx, token_next(*type))) {
		if (lexer_if(lx, TOKEN_IDENT, NULL)) {
			/* nothing */
		} else if (lexer_if(lx, TOKEN_LPAREN, NULL) &&
		    lexer_if(lx, TOKEN_STAR, NULL) &&
		    lexer_if(lx, TOKEN_IDENT, NULL) &&
		    lexer_if_pair(lx, TOKEN_LPAREN, TOKEN_RPAREN, NULL) &&
		    lexer_if(lx, TOKEN_RPAREN, NULL)) {
			/*
			 * Function returning a function pointer, used by
			 * parser_func_proto().
			 */
			(*type)->tk_flags |= TOKEN_FLAG_TYPE_FUNC;
		} else {
			goto out;
		}

		if (!lexer_if_pair(lx, TOKEN_LPAREN, TOKEN_RPAREN, NULL))
			goto out;

		while (lexer_if(lx, TOKEN_ATTRIBUTE, NULL) &&
		    lexer_if_pair(lx, TOKEN_LPAREN, TOKEN_RPAREN, NULL))
			continue;

		if (lexer_if(lx, TOKEN_SEMI, NULL))
			peek = PARSER_FUNC_PEEK_DECL;
		else if (lexer_if(lx, TOKEN_LBRACE, NULL))
			peek = PARSER_FUNC_PEEK_IMPL;
		else if (parser_type_peek(pr, NULL, 0))
			peek = PARSER_FUNC_PEEK_IMPL;	/* K&R */
	}
out:
	lexer_peek_leave(lx, &s);
	return peek;
}

int
parser_func_decl(struct parser *pr, struct doc *dc, struct ruler *rl,
    const struct token *type)
{
	struct lexer *lx = pr->pr_lx;
	struct doc *out = NULL;
	struct token *tk;
	int error;

	error = parser_func_proto(pr, &out, &(struct parser_func_proto_arg){
	    .dc		= doc_alloc(DOC_CONCAT, doc_alloc(DOC_GROUP, dc)),
	    .rl		= rl,
	    .type	= type,
	});
	if (error & (FAIL | NONE))
		return parser_fail(pr);

	if (lexer_expect(lx, TOKEN_SEMI, &tk))
		doc_token(tk, out);

	return parser_good(pr);
}

int
parser_func_impl(struct parser *pr, struct doc *dc)
{
	struct ruler rl;
	struct token *type;
	int error;

	if (parser_func_peek(pr, &type) != PARSER_FUNC_PEEK_IMPL)
		return parser_none(pr);

	ruler_init(&rl, 1, RULER_ALIGN_FIXED);
	error = parser_func_impl1(pr, dc, &rl, type);
	if (error & GOOD)
		ruler_exec(&rl);
	ruler_free(&rl);
	return error;
}

int
parser_func_arg(struct parser *pr, struct doc *dc, struct doc **out,
    const struct token *rparen)
{
	struct doc *concat;
	struct lexer *lx = pr->pr_lx;
	struct token *pv = NULL;
	struct token *tk, *type;
	int error = 0;

	if (!parser_type_peek(pr, &type, PARSER_TYPE_ARG))
		return parser_none(pr);

	/*
	 * Let each argument begin with a soft line, causing a line to be
	 * emitted immediately if the argument does not fit instead of breaking
	 * the argument.
	 */
	concat = doc_alloc(DOC_CONCAT, doc_alloc(DOC_GROUP, dc));
	doc_alloc(DOC_SOFTLINE, concat);
	concat = doc_alloc(DOC_CONCAT, doc_alloc(DOC_OPTIONAL, concat));

	if (parser_type(pr, concat, type, NULL) & (FAIL | NONE))
		return parser_fail(pr);

	/* Put the argument identifier in its own group to trigger a refit. */
	concat = doc_alloc(DOC_CONCAT, doc_alloc(DOC_GROUP, concat));
	if (out != NULL)
		*out = concat;

	/* Put a line between the type and identifier when wanted. */
	if (type->tk_type != TOKEN_STAR &&
	    !lexer_peek_if(lx, TOKEN_COMMA, NULL) &&
	    !lexer_peek_if(lx, TOKEN_RPAREN, NULL) &&
	    !lexer_peek_if(lx, TOKEN_ATTRIBUTE, NULL))
		doc_alloc(DOC_LINE, concat);

	for (;;) {
		if (lexer_peek_if(lx, LEXER_EOF, NULL))
			return parser_fail(pr);

		if (parser_attributes(pr, concat, NULL, 0) & GOOD)
			break;

		if (lexer_if(lx, TOKEN_COMMA, &tk)) {
			doc_token(tk, concat);
			doc_alloc(DOC_LINE, concat);
			break;
		}
		if (lexer_peek(lx, &tk) && tk == rparen)
			break;

		if (!lexer_pop(lx, &tk)) {
			error = parser_fail(pr);
			break;
		}
		/* Identifiers must be separated. */
		if (pv != NULL && pv->tk_type == TOKEN_IDENT &&
		    tk->tk_type == TOKEN_IDENT)
			doc_alloc(DOC_LINE, concat);
		doc_token(tk, concat);
		pv = tk;
	}

	return error ? error : parser_good(pr);
}

static int
parser_func_impl1(struct parser *pr, struct doc *dc, struct ruler *rl,
    const struct token *type)
{
	struct lexer *lx = pr->pr_lx;
	struct doc *out = NULL;
	int error;

	error = parser_func_proto(pr, &out, &(struct parser_func_proto_arg){
	    .dc		= dc,
	    .rl		= rl,
	    .type	= type,
	    .flags	= PARSER_EXEC_FUNC_PROTO_IMPL,
	});
	if (error & (FAIL | NONE))
		return parser_fail(pr);
	if (!lexer_peek_if(lx, TOKEN_LBRACE, NULL))
		return parser_fail(pr);

	if (style_brace_wrapping(pr->pr_st, AfterFunction))
		doc_alloc(DOC_HARDLINE, dc);
	else
		doc_literal(" ", dc);
	error = parser_stmt_block(pr, &(struct parser_stmt_block_arg){
	    .head	= dc,
	    .tail	= dc,
	});
	if (error & (FAIL | NONE))
		return parser_fail(pr);
	doc_alloc(DOC_HARDLINE, dc);
	if (peek_impl_line(pr))
		doc_alloc(DOC_HARDLINE, dc);

	return parser_good(pr);
}

/*
 * Parse a function prototype, i.e. return type, identifier, arguments and
 * optional attributes. The caller is expected to already have parsed the
 * return type.
 */
static int
parser_func_proto(struct parser *pr, struct doc **out,
    struct parser_func_proto_arg *arg)
{
	struct doc *dc = arg->dc;
	struct doc *attributes, *concat, *group, *indent, *kr;
	struct lexer *lx = pr->pr_lx;
	struct token *lparen, *rparen, *tk;
	unsigned int s;
	int nkr = 0;
	int error;

	if (parser_type(pr, dc, arg->type, arg->rl) & (FAIL | NONE))
		return parser_fail(pr);

	s = style(pr->pr_st, AlwaysBreakAfterReturnType);
	if ((s == All || s == TopLevel) ||
	    ((arg->flags & PARSER_EXEC_FUNC_PROTO_IMPL) &&
	     (s == AllDefinitions || s == TopLevelDefinitions)))
		doc_alloc(DOC_HARDLINE, dc);

	/*
	 * The function identifier and arguments are intended to fit on a single
	 * line.
	 */
	group = doc_alloc(DOC_GROUP, dc);
	concat = doc_alloc(DOC_CONCAT, group);

	if (arg->type->tk_flags & TOKEN_FLAG_TYPE_FUNC) {
		/* Function returning function pointer. */
		if (lexer_expect(lx, TOKEN_LPAREN, &lparen))
			doc_token(lparen, concat);
		if (lexer_expect(lx, TOKEN_STAR, &tk))
			doc_token(tk, concat);
		if (lexer_expect(lx, TOKEN_IDENT, &tk))
			doc_token(tk, concat);
		if (!lexer_peek_if_pair(lx, TOKEN_LPAREN, TOKEN_RPAREN,
		    &rparen))
			return parser_fail(pr);
		if (lexer_expect(lx, TOKEN_LPAREN, &lparen))
			doc_token(lparen, concat);
		while (parser_func_arg(pr, concat, NULL, rparen) & GOOD)
			continue;
		if (lexer_expect(lx, TOKEN_RPAREN, &rparen))
			doc_token(rparen, concat);
		if (lexer_expect(lx, TOKEN_RPAREN, &rparen))
			doc_token(rparen, concat);
	} else if (lexer_expect(lx, TOKEN_IDENT, &tk)) {
		parser_token_trim_after(pr, tk);
		doc_token(tk, concat);
	} else {
		doc_remove(group, dc);
		return parser_fail(pr);
	}

	if (!lexer_peek_if_pair(lx, TOKEN_LPAREN, TOKEN_RPAREN, &rparen))
		return parser_fail(pr);
	if (lexer_expect(lx, TOKEN_LPAREN, &lparen)) {
		parser_token_trim_after(pr, lparen);
		parser_token_trim_before(pr, rparen);
		parser_token_trim_after(pr, rparen);
		doc_token(lparen, concat);
	}
	if (style(pr->pr_st, AlignAfterOpenBracket) == Align) {
		indent = doc_alloc_indent(DOC_INDENT_WIDTH, concat);
	} else {
		indent = doc_alloc_indent(
		    style(pr->pr_st, ContinuationIndentWidth), concat);
	}
	while (parser_func_arg(pr, indent, out, rparen) & GOOD)
		continue;
	/* Can be empty if arguments are absent. */
	if (*out == NULL)
		*out = concat;
	if (lexer_expect(lx, TOKEN_RPAREN, &rparen))
		doc_token(rparen, *out);

	/* Recognize K&R argument declarations. */
	kr = doc_alloc(DOC_GROUP, dc);
	indent = doc_alloc_indent(style(pr->pr_st, IndentWidth), kr);
	doc_alloc(DOC_HARDLINE, indent);
	if (parser_decl(pr, indent, 0) & GOOD)
		nkr++;
	if (nkr == 0)
		doc_remove(kr, dc);

	attributes = doc_alloc(DOC_GROUP, dc);
	indent = doc_alloc_indent(style(pr->pr_st, IndentWidth), attributes);
	error = parser_attributes(pr, indent, out, PARSER_ATTRIBUTES_HARDLINE);
	if (error & HALT)
		doc_remove(attributes, dc);

	return parser_good(pr);
}

/*
 * Returns non-zero if the right brace of a function implementation can be
 * followed by a hard line.
 */
static int
peek_impl_line(struct parser *pr)
{
	struct lexer *lx = pr->pr_lx;
	struct token *cpp, *ident, *rbrace;
	struct lexer_state s;
	int annotated = 0;

	if (lexer_peek_if(lx, LEXER_EOF, NULL) || !lexer_back(lx, &rbrace))
		return 0;

	if (lexer_peek_if_prefix_flags(lx, TOKEN_FLAG_CPP, &cpp))
		return cpp->tk_lno - rbrace->tk_lno > 1;

	lexer_peek_enter(lx, &s);
	if ((lexer_if(lx, TOKEN_IDENT, &ident) ||
	    lexer_if(lx, TOKEN_ASSEMBLY, &ident)) &&
	    lexer_if_pair(lx, TOKEN_LPAREN, TOKEN_RPAREN, NULL) &&
	    lexer_if(lx, TOKEN_SEMI, NULL) &&
	    ident->tk_lno - rbrace->tk_lno == 1)
		annotated = 1;
	lexer_peek_leave(lx, &s);
	if (annotated)
		return 0;

	return 1;
}

#include "parser-braces.h"

#include "doc.h"
#include "expr.h"
#include "lexer.h"
#include "parser-cpp.h"
#include "parser-expr.h"
#include "parser-priv.h"
#include "ruler.h"
#include "token.h"

struct braces_arg {
	struct doc	*dc;
	struct ruler	*rl;
	unsigned int	 indent;
	unsigned int	 col;
	unsigned int	 flags;
};

struct braces_field_arg {
	struct doc		*dc;
	struct ruler		*rl;
	const struct token	*rbrace;
	unsigned int		 indent;
	unsigned int		 flags;
};

static int	parser_braces1(struct parser *, struct braces_arg *);
static int	parser_braces_field(struct parser *, struct braces_field_arg *);
static int	parser_braces_field1(struct parser *,
    struct braces_field_arg *);

static struct token	*peek_expr_stop(struct parser *, struct token *);
static int		 peek_hardline(struct parser *, const struct token *);
static struct token	*lbrace_cache(struct parser *, struct token *);
static void		 lbrace_cache_purge(struct parser *);

int
parser_braces(struct parser *pr, struct doc *dc, unsigned int indent,
    unsigned int flags)
{
	struct ruler rl;
	struct doc *concat;
	int error;

	ruler_init(&rl, 0, RULER_ALIGN_TABS | RULER_REQUIRE_TABS);
	concat = doc_alloc(DOC_CONCAT, dc);
	error = parser_braces1(pr, &(struct braces_arg){
	    .dc		= concat,
	    .rl		= &rl,
	    .indent	= indent,
	    .col	= 0,
	    .flags	= flags,
	});
	lbrace_cache_purge(pr);
	ruler_exec(&rl);
	ruler_free(&rl);
	return error;
}

static int
parser_braces1(struct parser *pr, struct braces_arg *arg)
{
	struct doc *braces, *indent;
	struct lexer *lx = pr->pr_lx;
	struct token *lbrace, *rbrace, *tk;
	unsigned int w = 0;
	int align = 1;
	int error, hasline;

	if (!lexer_peek_if_pair(lx, TOKEN_LBRACE, TOKEN_RBRACE, &rbrace))
		return parser_fail(pr);

	lbrace_cache_purge(pr);

	/*
	 * If any column is followed by a hard line, do not align but
	 * instead respect existing hard line(s).
	 */
	if (peek_hardline(pr, rbrace))
		align = 0;

	braces = doc_alloc(DOC_CONCAT, arg->dc);

	if (!lexer_expect(lx, TOKEN_LBRACE, &lbrace))
		return parser_fail(pr);
	hasline = token_has_line(lbrace, 1);
	if (arg->flags & PARSER_EXEC_DECL_BRACES_TRIM)
		parser_token_trim_after(pr, lbrace);
	doc_token(lbrace, braces);

	if (lexer_peek_if(lx, TOKEN_RBRACE, NULL)) {
		/* Honor spaces in empty braces. */
		if (token_has_spaces(lbrace))
			doc_literal(" ", braces);
		goto out;
	}

	if (hasline) {
		int val = arg->indent;

		if (arg->flags & PARSER_EXEC_DECL_BRACES_INDENT_MAYBE)
			val |= DOC_INDENT_NEWLINE;
		indent = doc_alloc_indent(val, braces);
		doc_alloc(DOC_HARDLINE, indent);
	} else {
		if (token_has_spaces(lbrace))
			doc_literal(" ", braces);

		/*
		 * Take note of the width of the document, must be accounted for
		 * while performing alignment.
		 */
		w = parser_width(pr, braces);
		indent = doc_alloc_indent(w, braces);
	}

	for (;;) {
		struct doc *expr = NULL;
		struct doc *concat;
		struct token *comma, *nx, *pv;

		if (!lexer_peek(lx, &tk) || tk->tk_type == LEXER_EOF)
			return parser_fail(pr);
		if (tk == rbrace)
			break;

		concat = doc_alloc(DOC_CONCAT, doc_alloc(DOC_GROUP, indent));

		if ((arg->flags & PARSER_EXEC_DECL_BRACES_ENUM) ||
		    lexer_peek_if(lx, TOKEN_PERIOD, NULL) ||
		    lexer_peek_if(lx, TOKEN_LSQUARE, NULL)) {
			error = parser_braces_field(pr,
			    &(struct braces_field_arg){
				.dc	= concat,
				.rl	= arg->rl,
				.rbrace	= rbrace,
				.indent	= arg->indent,
				.flags	= arg->flags,
			});
			if (error & HALT)
				return parser_fail(pr);
		} else if (lexer_peek_if(lx, TOKEN_LBRACE, &nx)) {
			unsigned int rmflags = PARSER_EXEC_DECL_BRACES_DEDENT;
			struct braces_arg newarg = {
				.dc	= concat,
				.rl	= arg->rl,
				.indent	= arg->indent,
				.col	= arg->col,
				.flags	= arg->flags & ~rmflags,
			};

			if (parser_braces1(pr, &newarg) & HALT)
				return parser_fail(pr);
			/*
			 * If the nested braces are positioned on the same line
			 * as the braces currently being parsed, inherit the
			 * column as we're still on the same row in terms of
			 * alignment.
			 */
			if (token_cmp(lbrace, nx) == 0)
				arg->col = newarg.col;
		} else {
			struct token *stop;

			stop = peek_expr_stop(pr, rbrace);
			error = parser_expr(pr, &expr,
			    &(struct parser_expr_arg){
				.dc	= concat,
				.rl	= arg->rl,
				.stop	= stop,
				.flags	= EXPR_EXEC_ALIGN,
			});
			if (error & HALT)
				return parser_fail(pr);
		}
		if (lexer_if(lx, TOKEN_COMMA, &comma)) {
			if (expr == NULL)
				expr = concat;
			doc_token(comma, expr);

			if (align) {
				arg->col++;
				w += parser_width(pr, concat);
				ruler_insert(arg->rl, comma, concat,
				    arg->col, w, 0);
				w = 0;
				goto next;
			}
		}

		if (!lexer_back(lx, &pv) || !lexer_peek(lx, &nx)) {
			return parser_fail(pr);
		} else if (token_has_spaces(pv) &&
		    (nx != rbrace || token_cmp(pv, rbrace) == 0)) {
			doc_literal(" ", concat);
		} else if (nx == rbrace) {
			/*
			 * Put the last hard line outside to get indentation
			 * right.
			 */
			if (token_cmp(pv, rbrace) < 0) {
				if (arg->flags &
				    PARSER_EXEC_DECL_BRACES_DEDENT) {
					braces = doc_alloc_indent(-arg->indent,
					    braces);
				}
				doc_alloc(DOC_HARDLINE, braces);
			}
		} else if (pv->tk_type == TOKEN_RPAREN &&
		    !token_has_line(pv, 1)) {
			/*
			 * Probably a cast followed by braces initializers, no
			 * hard line wanted.
			 */
		} else {
			doc_alloc(DOC_HARDLINE, indent);
		}

next:
		if (((arg->flags & PARSER_EXEC_DECL_BRACES_ENUM) == 0) &&
		    lexer_back(lx, &nx) && token_has_line(nx, 2))
			ruler_exec(arg->rl);
	}

out:
	if (lexer_expect(lx, TOKEN_RBRACE, &rbrace)) {
		parser_token_trim_after(pr, rbrace);
		doc_token(rbrace, braces);
	}
	if (!lexer_peek_if(lx, TOKEN_SEMI, NULL) &&
	    !lexer_peek_if(lx, TOKEN_COMMA, NULL) &&
	    !lexer_peek_if(lx, TOKEN_RBRACE, NULL) &&
	    !lexer_peek_if(lx, TOKEN_RPAREN, NULL))
		doc_literal(" ", braces);

	lbrace_cache_purge(pr);

	return parser_good(pr);
}

static int
parser_braces_field(struct parser *pr, struct braces_field_arg *arg)
{
	struct lexer *lx = pr->pr_lx;
	struct doc *dc = arg->dc;
	struct token *equal;
	int nfields = 0;
	int error;

	for (;;) {
		error = parser_braces_field1(pr, arg);
		if (error & NONE)
			break;
		if (error & HALT)
			return parser_fail(pr);
		nfields++;
	}
	if (nfields == 0)
		return parser_fail(pr);

	if (lexer_if(lx, TOKEN_EQUAL, &equal)) {
		struct token *stop;

		ruler_insert(arg->rl, token_prev(equal), dc, 1,
		    parser_width(pr, dc), 0);

		doc_token(equal, dc);
		doc_literal(" ", dc);

		lexer_peek_until_comma(lx, arg->rbrace, &stop);
		error = parser_expr(pr, NULL, &(struct parser_expr_arg){
		    .dc		= dc,
		    .stop	= stop,
		    .indent	= arg->indent,
		    .flags	= token_has_line(equal, 1) ?
			EXPR_EXEC_HARDLINE : 0,
		});
		if (error & HALT)
			return parser_fail(pr);
	}

	return parser_good(pr);
}

static int
parser_braces_field1(struct parser *pr, struct braces_field_arg *arg)
{
	struct lexer *lx = pr->pr_lx;
	struct doc *dc = arg->dc;
	struct token *tk;
	int error;

	if (lexer_if(lx, TOKEN_LSQUARE, &tk)) {
		struct doc *expr = NULL;

		doc_token(tk, dc);
		error = parser_expr(pr, &expr, &(struct parser_expr_arg){
		    .dc	= dc,
		});
		if (error & HALT)
			return parser_fail(pr);
		if (lexer_expect(lx, TOKEN_RSQUARE, &tk))
			doc_token(tk, expr);
		return parser_good(pr);
	} else if (lexer_if(lx, TOKEN_PERIOD, &tk)) {
		struct token *equal;

		doc_token(tk, dc);
		if (lexer_expect(lx, TOKEN_IDENT, &tk))
			doc_token(tk, dc);

		/* Correct alignment, must occur after the ident. */
		if (lexer_peek_if(lx, TOKEN_EQUAL, &equal) &&
		    token_has_tabs(equal))
			token_move_suffixes_if(equal, tk, TOKEN_SPACE);

		return parser_good(pr);
	} else if (lexer_if(lx, TOKEN_IDENT, &tk)) {
		doc_token(tk, dc);

		/* Enum making use of preprocessor directives. */
		if ((arg->flags & PARSER_EXEC_DECL_BRACES_ENUM) &&
		    lexer_if(lx, TOKEN_LPAREN, &tk)) {
			struct doc *expr = NULL;

			doc_token(tk, dc);
			error = parser_expr(pr, &expr,
			    &(struct parser_expr_arg){
				.dc	= dc,
			});
			if (error & FAIL)
				return parser_fail(pr);
			if (error & HALT)
				expr = dc;
			if (lexer_expect(lx, TOKEN_RPAREN, &tk))
				doc_token(tk, expr);
		}

		return parser_good(pr);
	}

	return parser_none(pr);
}

static struct token *
peek_expr_stop(struct parser *pr, struct token *rbrace)
{
	struct lexer *lx = pr->pr_lx;
	struct token *comma, *rparen, *stop;

	if (parser_cpp_peek_x(pr, &rparen))
		return token_next(rparen);

	/*
	 * Finding the next lbrace can be costly if the current pair of braces
	 * has many entries. Therefore utilize a tiny cache.
	 */
	stop = lbrace_cache(pr, rbrace);
	if (lexer_peek_until_comma(lx, stop, &comma))
		return comma;
	return stop;
}

/*
 * Returns non-zero if any token up to the given stop token exclusively is
 * followed by a hard line.
 */
static int
peek_hardline(struct parser *pr, const struct token *stop)
{
	struct lexer_state s;
	struct lexer *lx = pr->pr_lx;
	struct token *pv = NULL;
	int peek = 0;

	lexer_peek_enter(lx, &s);
	for (;;) {
		struct token *tk;

		if (!lexer_pop(lx, &tk))
			return parser_fail(pr);
		if (tk == stop)
			break;

		if (pv != NULL && token_cmp(tk, pv) > 0) {
			peek = 1;
			break;
		}
		pv = tk;
	}
	lexer_peek_leave(lx, &s);

	return peek;
}

static struct token *
lbrace_cache(struct parser *pr, struct token *fallback)
{
	if (!pr->pr_braces.valid) {
		struct token *lbrace = NULL;

		lbrace_cache_purge(pr);
		lexer_peek_until(pr->pr_lx, TOKEN_LBRACE, &lbrace);
		if (lbrace != NULL)
			token_ref(lbrace);
		pr->pr_braces.lbrace = lbrace;
		pr->pr_braces.valid = 1;
	}
	return pr->pr_braces.lbrace != NULL ? pr->pr_braces.lbrace : fallback;
}

static void
lbrace_cache_purge(struct parser *pr)
{
	if (pr->pr_braces.lbrace != NULL)
		token_rele(pr->pr_braces.lbrace);
	pr->pr_braces.lbrace = NULL;
	pr->pr_braces.valid = 0;
}
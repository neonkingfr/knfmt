#include "parser-type.h"

#include "config.h"

#include <string.h>

#include "cdefs.h"
#include "doc.h"
#include "lexer.h"
#include "parser-cpp.h"
#include "parser-func.h"
#include "parser-priv.h"
#include "ruler.h"
#include "style.h"
#include "token.h"

static int	lexer_peek_if_func_ptr(struct lexer *, struct token **);
static int	lexer_peek_if_ptr(struct lexer *, struct token **);
static int	lexer_peek_if_type_ident(struct lexer *lx);
static int	peek_type_noident(struct lexer *, struct token **);

int
parser_type_peek(struct parser *pr, struct parser_type *type,
    unsigned int flags)
{
	struct lexer *lx = pr->pr_lx;
	struct lexer_state s;
	struct token *align = NULL;
	struct token *beg, *pv, *t;
	int peek = 0;
	int nkeywords = 0;
	int ntokens = 0;
	int unknown = 0;
	int issizeof;

	if (!lexer_peek(lx, &beg))
		return 0;
	issizeof = lexer_back(lx, &pv) && pv->tk_type == TOKEN_SIZEOF;

	/*
	 * Recognize function argument consisting of a single type and no
	 * variable name.
	 */
	if ((flags & (PARSER_TYPE_CAST | PARSER_TYPE_ARG)) &&
	    peek_type_noident(lx, &t)) {
		peek = 1;
		goto out;
	}

	lexer_peek_enter(lx, &s);
	for (;;) {
		struct token *rparen;

		if (lexer_peek_if(lx, LEXER_EOF, NULL))
			break;

		if (lexer_if_flags(lx,
		    TOKEN_FLAG_QUALIFIER | TOKEN_FLAG_STORAGE, &t)) {
			if (lexer_if(lx, TOKEN_LPAREN, NULL)) {
				peek = 0;
				ntokens = 0;
				break;
			}
			nkeywords++;
			peek = 1;
		} else if (lexer_if_flags(lx, TOKEN_FLAG_TYPE, &t)) {
			if (t->tk_type == TOKEN_ENUM ||
			    t->tk_type == TOKEN_STRUCT ||
			    t->tk_type == TOKEN_UNION)
				(void)lexer_if(lx, TOKEN_IDENT, &t);
			/* Recognize constructs like `struct s[]'. */
			(void)lexer_if_pair(lx, TOKEN_LSQUARE, TOKEN_RSQUARE,
			    &t);
			peek = 1;
		} else if (lexer_if(lx, TOKEN_STAR, &t)) {
			/*
			 * A pointer is expected to only be followed by another
			 * pointer or a known type. Otherwise, the following
			 * identifier cannot be part of the type.
			 */
			if (lexer_peek_if(lx, TOKEN_IDENT, NULL))
				break;
			/* A type cannot start with a pointer. */
			if (ntokens == 0)
				break;
			peek = 1;
		} else if (parser_cpp_peek_type(pr, &rparen)) {
			t = rparen;
			lexer_seek(lx, token_next(rparen));
		} else if (lexer_peek_if(lx, TOKEN_IDENT, NULL)) {
			/* Ensure this is not the identifier after the type. */
			if ((flags & PARSER_TYPE_CAST) == 0 &&
			    (flags & PARSER_TYPE_EXPR) == 0 &&
			    lexer_peek_if_type_ident(lx))
				break;

			/* Identifier is part of the type, consume it. */
			if (!lexer_if(lx, TOKEN_IDENT, &t))
				break;
		} else if (ntokens > 0 && lexer_peek_if_func_ptr(lx, &t)) {
			if (lexer_back(lx, &align))
				peek = 1;
			break;
		} else if (ntokens > 0 && lexer_peek_if_ptr(lx, &t)) {
			peek = 1;
			break;
		} else {
			unknown = 1;
			break;
		}

		ntokens++;
		if (issizeof)
			break;
	}
	lexer_peek_leave(lx, &s);

	if (ntokens > 0 && ntokens == nkeywords &&
	    (flags & PARSER_TYPE_ARG) == 0) {
		/* Only qualifier or storage token(s) cannot denote a type. */
		peek = 0;
	} else if (!peek && !unknown && ntokens > 0) {
		/*
		 * Nothing was found. However this is a sequence of identifiers
		 * (i.e. unknown types) therefore treat it as a type.
		 */
		peek = 1;
	}

out:
	if (peek && type != NULL) {
		*type = (struct parser_type){
		    .end	= t,
		    .align	= align,
		};
	}
	return peek;
}

int
parser_type(struct parser *pr, struct doc *dc, struct parser_type *type,
    struct ruler *rl)
{
	struct lexer *lx = pr->pr_lx;
	const struct token *align = NULL;
	const struct token *end = type->end;
	struct token *beg;
	unsigned int nspaces = 0;

	if (!lexer_peek(lx, &beg))
		return parser_fail(pr);

	if (rl != NULL) {
		/*
		 * Find the first non pointer token starting from the end, this
		 * is where the ruler alignment must be performed.
		 */
		align = type->align != NULL ? type->align : end;
		for (;;) {
			if (align->tk_type != TOKEN_STAR)
				break;

			nspaces++;
			if (align == beg)
				break;
			align = token_prev(align);
			if (align == NULL)
				break;
		}

		/*
		 * No alignment wanted if the first non-pointer token is
		 * followed by a semi.
		 */
		if (align != NULL) {
			const struct token *nx;

			nx = token_next(align);
			if (nx != NULL && nx->tk_type == TOKEN_SEMI)
				align = NULL;
		}
	}

	for (;;) {
		struct doc *concat;
		struct token *tk;
		int didalign = 0;

		if (!lexer_pop(lx, &tk))
			return parser_fail(pr);
		parser_token_trim_after(pr, tk);

		if (tk->tk_flags & TOKEN_FLAG_TYPE_ARGS) {
			struct doc *indent;
			struct token *lparen = tk;
			struct token *rparen;
			unsigned int w;

			doc_token(lparen, dc);
			if (style(pr->pr_st, AlignAfterOpenBracket) == Align)
				w = parser_width(pr, dc);
			else
				w = style(pr->pr_st, ContinuationIndentWidth);
			indent = doc_alloc_indent(w, dc);
			while (parser_func_arg(pr, indent, NULL, end) & GOOD)
				continue;
			if (lexer_expect(lx, TOKEN_RPAREN, &rparen))
				doc_token(rparen, dc);
			break;
		}

		concat = doc_alloc(DOC_CONCAT, doc_alloc(DOC_GROUP, dc));
		doc_token(tk, concat);

		if (tk == align) {
			if (token_is_decl(tk, TOKEN_ENUM) ||
			    token_is_decl(tk, TOKEN_STRUCT) ||
			    token_is_decl(tk, TOKEN_UNION)) {
				doc_alloc(DOC_LINE, concat);
			} else {
				ruler_insert(rl, tk, concat, 1,
				    parser_width(pr, dc), nspaces);
			}
			didalign = 1;
		}

		if (tk == end)
			break;

		if (!didalign) {
			struct lexer_state s;
			struct token *nx;

			lexer_peek_enter(lx, &s);
			if (tk->tk_type != TOKEN_STAR &&
			    tk->tk_type != TOKEN_LPAREN &&
			    tk->tk_type != TOKEN_LSQUARE &&
			    lexer_pop(lx, &nx) &&
			    (nx->tk_type != TOKEN_LPAREN ||
			     lexer_if(lx, TOKEN_STAR, NULL)) &&
			    nx->tk_type != TOKEN_LSQUARE &&
			    nx->tk_type != TOKEN_RSQUARE &&
			    nx->tk_type != TOKEN_RPAREN &&
			    nx->tk_type != TOKEN_COMMA)
				doc_alloc(DOC_LINE, concat);
			lexer_peek_leave(lx, &s);
		}
	}

	return parser_good(pr);
}

static int
lexer_peek_if_func_ptr(struct lexer *lx, struct token **tk)
{
	struct lexer_state s;
	struct token *lparen, *rparen;
	int peek = 0;

	lexer_peek_enter(lx, &s);
	if (lexer_if(lx, TOKEN_LPAREN, NULL) &&
	    lexer_if(lx, TOKEN_STAR, NULL)) {
		struct token *ident = NULL;

		while (lexer_if(lx, TOKEN_STAR, NULL))
			continue;

		lexer_if_flags(lx, TOKEN_FLAG_QUALIFIER, NULL);
		lexer_if(lx, TOKEN_IDENT, &ident);
		if (lexer_if(lx, TOKEN_LSQUARE, NULL)) {
			lexer_if(lx, TOKEN_LITERAL, NULL);
			lexer_if(lx, TOKEN_RSQUARE, NULL);
		}
		if (lexer_if(lx, TOKEN_RPAREN, &rparen)) {
			if (lexer_peek_if(lx, TOKEN_LPAREN, &lparen) &&
			    lexer_if_pair(lx, TOKEN_LPAREN, TOKEN_RPAREN, tk)) {
				/*
				 * Annotate the left parenthesis, used by
				 * parser_exec_type().
				 */
				lparen->tk_flags |= TOKEN_FLAG_TYPE_ARGS;

				peek = 1;
			} else if (ident == NULL &&
			    (lexer_if(lx, TOKEN_RPAREN, NULL) ||
			     lexer_if(lx, LEXER_EOF, NULL))) {
				/*
				 * Function pointer lacking arguments wrapped in
				 * parenthesis. Be careful here in order to not
				 * confuse a function call.
				 */
				peek = 1;
				*tk = rparen;
			}
		}
	}
	lexer_peek_leave(lx, &s);

	return peek;
}

static int
lexer_peek_if_ptr(struct lexer *lx, struct token **UNUSED(tk))
{
	struct lexer_state s;
	int peek = 0;

	lexer_peek_enter(lx, &s);
	if (lexer_if(lx, TOKEN_LPAREN, NULL) &&
	    lexer_if(lx, TOKEN_STAR, NULL) &&
	    lexer_if(lx, TOKEN_IDENT, NULL) &&
	    lexer_if(lx, TOKEN_RPAREN, NULL) &&
	    lexer_if(lx, TOKEN_LSQUARE, NULL))
		peek = 1;
	lexer_peek_leave(lx, &s);
	return peek;
}

static int
lexer_peek_if_type_ident(struct lexer *lx)
{
	struct lexer_state s;
	int peek = 0;

	lexer_peek_enter(lx, &s);
	if (lexer_if(lx, TOKEN_IDENT, NULL) &&
	    (lexer_if_flags(lx, TOKEN_FLAG_ASSIGN, NULL) ||
	     lexer_if(lx, TOKEN_LSQUARE, NULL) ||
	     (lexer_if(lx, TOKEN_LPAREN, NULL) &&
	      !lexer_peek_if(lx, TOKEN_STAR, NULL)) ||
	     lexer_if(lx, TOKEN_RPAREN, NULL) ||
	     lexer_if(lx, TOKEN_SEMI, NULL) ||
	     lexer_if(lx, TOKEN_COMMA, NULL) ||
	     lexer_if(lx, TOKEN_COLON, NULL) ||
	     lexer_if(lx, TOKEN_ATTRIBUTE, NULL)))
		peek = 1;
	lexer_peek_leave(lx, &s);

	return peek;
}

static int
peek_type_noident(struct lexer *lx, struct token **tk)
{
	struct lexer_state s;
	int peek = 0;

	lexer_peek_enter(lx, &s);
	if (lexer_if(lx, TOKEN_IDENT, tk) &&
	    (lexer_if(lx, TOKEN_RPAREN, NULL) ||
	     lexer_if(lx, TOKEN_COMMA, NULL)))
		peek = 1;
	lexer_peek_leave(lx, &s);
	return peek;
}

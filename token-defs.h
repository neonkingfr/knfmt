#define F(f) TOKEN_FLAG_ ## f

#ifndef A
#define A(t, s, f)
#endif

#ifndef S
#define S(t, s, f)
#endif

/* keywords */
T(TOKEN_ASSEMBLY,	"asm",			0)
T(TOKEN_ATTRIBUTE,	"__attribute__",	0)
T(TOKEN_BREAK,		"break",		0)
T(TOKEN_CASE,		"case",			0)
T(TOKEN_CHAR,		"char",			F(TYPE))
T(TOKEN_CONST,		"const",		F(QUALIFIER))
T(TOKEN_CONTINUE,	"continue",		0)
T(TOKEN_DEFAULT,	"default",		0)
T(TOKEN_DO,		"do",			0)
T(TOKEN_DOUBLE,		"double",		F(TYPE))
T(TOKEN_ELSE,		"else",			0)
T(TOKEN_ENUM,		"enum",			F(TYPE))
T(TOKEN_EXTERN,		"extern",		F(STORAGE))
T(TOKEN_FLOAT,		"float",		F(TYPE))
T(TOKEN_FOR,		"for",			0)
T(TOKEN_GOTO,		"goto",			0)
T(TOKEN_IF,		"if",			0)
T(TOKEN_INLINE,		"inline",		F(STORAGE))
T(TOKEN_INT,		"int",			F(TYPE))
T(TOKEN_LONG,		"long",			F(TYPE))
T(TOKEN_REGISTER,	"register",		F(STORAGE))
T(TOKEN_RESTRICT,	"restrict",		F(QUALIFIER))
T(TOKEN_RETURN,		"return",		0)
T(TOKEN_SHORT,		"short",		F(TYPE))
T(TOKEN_SIGNED,		"signed",		F(TYPE))
T(TOKEN_SIZEOF,		"sizeof",		0)
T(TOKEN_STATIC,		"static",		F(STORAGE))
T(TOKEN_STRUCT,		"struct",		F(TYPE))
T(TOKEN_SWITCH,		"switch",		0)
T(TOKEN_TYPEDEF,	"typedef",		F(TYPE))
T(TOKEN_UNION,		"union",		F(TYPE))
T(TOKEN_UNSIGNED,	"unsigned",		F(TYPE))
T(TOKEN_VOID,		"void",			F(TYPE))
T(TOKEN_VOLATILE,	"volatile",		F(QUALIFIER))
T(TOKEN_WHILE,		"while",		0)

/* keyword aliases. */
A(TOKEN_ASSEMBLY,	"__asm",	0)
A(TOKEN_ASSEMBLY,	"__asm__",	0)
A(TOKEN_ATTRIBUTE,	"__attribute",	0)
A(TOKEN_RESTRICT,	"__restrict",	F(QUALIFIER))
A(TOKEN_VOLATILE,	"__volatile",	F(QUALIFIER))
A(TOKEN_VOLATILE,	"__volatile__",	F(QUALIFIER))

/* punctuators */
T(TOKEN_LSQUARE,		"[",	0)
T(TOKEN_RSQUARE,		"]",	0)
T(TOKEN_LPAREN,			"(",	0)
T(TOKEN_RPAREN,			")",	0)
T(TOKEN_LBRACE,			"{",	0)
T(TOKEN_RBRACE,			"}",	0)
T(TOKEN_PERIOD,			".",	F(AMBIGUOUS))
T(TOKEN_ELLIPSIS,		"...",	F(TYPE))
T(TOKEN_AMP,			"&",	F(AMBIGUOUS) | F(BINARY))
T(TOKEN_AMPAMP,			"&&",	F(BINARY))
T(TOKEN_AMPEQUAL,		"&=",	F(ASSIGN))
T(TOKEN_STAR,			"*",	F(AMBIGUOUS) | F(BINARY) | F(SPACE))
T(TOKEN_STAREQUAL,		"*=",	F(ASSIGN))
T(TOKEN_PLUS,			"+",	F(AMBIGUOUS) | F(BINARY))
T(TOKEN_PLUSPLUS,		"++",	0)
T(TOKEN_PLUSEQUAL,		"+=",	F(ASSIGN))
T(TOKEN_MINUS,			"-",	F(AMBIGUOUS) | F(BINARY))
T(TOKEN_ARROW,			"->",	0)
T(TOKEN_MINUSMINUS,		"--",	0)
T(TOKEN_MINUSEQUAL,		"-=",	F(ASSIGN))
T(TOKEN_TILDE,			"~",	0)
T(TOKEN_EXCLAIM,		"!",	F(AMBIGUOUS))
T(TOKEN_EXCLAIMEQUAL,		"!=",	F(BINARY))
T(TOKEN_SLASH,			"/",	F(AMBIGUOUS) | F(BINARY) | F(SPACE))
T(TOKEN_SLASHEQUAL,		"/=",	F(ASSIGN))
T(TOKEN_PERCENT,		"%",	F(AMBIGUOUS) | F(BINARY))
T(TOKEN_PERCENTEQUAL,		"%=",	F(ASSIGN))
T(TOKEN_LESS,			"<",	F(AMBIGUOUS) | F(BINARY))
T(TOKEN_LESSLESS,		"<<",	F(AMBIGUOUS) | F(BINARY))
T(TOKEN_LESSEQUAL,		"<=",	F(BINARY))
T(TOKEN_LESSLESSEQUAL,		"<<=",	F(ASSIGN))
T(TOKEN_GREATER,		">",	F(AMBIGUOUS) | F(BINARY))
T(TOKEN_GREATERGREATER,		">>",	F(AMBIGUOUS) | F(BINARY))
T(TOKEN_GREATEREQUAL,		">=",	F(BINARY))
T(TOKEN_GREATERGREATEREQUAL,	">>=",	F(ASSIGN))
T(TOKEN_CARET,			"^",	F(AMBIGUOUS))
T(TOKEN_CARETEQUAL,		"^=",	F(ASSIGN))
T(TOKEN_PIPE,			"|",	F(AMBIGUOUS) | F(BINARY) | F(SPACE))
T(TOKEN_PIPEPIPE,		"||",	F(BINARY))
T(TOKEN_PIPEEQUAL,		"|=",	F(ASSIGN))
T(TOKEN_QUESTION,		"?",	0)
T(TOKEN_COLON,			":",	0)
T(TOKEN_SEMI,			";",	0)
T(TOKEN_EQUAL,			"=",	F(AMBIGUOUS) | F(ASSIGN))
T(TOKEN_EQUALEQUAL,		"==",	F(BINARY))
T(TOKEN_COMMA,			",",	0)
T(TOKEN_BACKSLASH,		"\\",	F(DISCARD))

/* sentinels */
S(TOKEN_COMMENT,	"",	0)
S(TOKEN_CPP,		"",	0)
S(TOKEN_CPP_ELSE,	"",	0)
S(TOKEN_CPP_ENDIF,	"",	0)
S(TOKEN_CPP_IF,		"",	0)
S(TOKEN_CPP_INCLUDE,	"",	0)
S(TOKEN_IDENT,		"",	0)
S(TOKEN_LITERAL,	"",	0)
S(TOKEN_SPACE,		"",	0)
S(TOKEN_STRING,		"",	0)

/* last */
T(TOKEN_NONE,	"",	0)

#undef A
#undef F
#undef S
#undef T

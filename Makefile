include ${.CURDIR}/config.mk

VERSION=	4.1.1

SRCS+=	alloc.c
SRCS+=	arithmetic.c
SRCS+=	buffer.c
SRCS+=	clang.c
SRCS+=	comment.c
SRCS+=	compat-errc.c
SRCS+=	compat-pledge.c
SRCS+=	compat-warnc.c
SRCS+=	consistency.c
SRCS+=	cpp-align.c
SRCS+=	cpp-include.c
SRCS+=	diff.c
SRCS+=	doc.c
SRCS+=	error.c
SRCS+=	expr.c
SRCS+=	file.c
SRCS+=	lexer.c
SRCS+=	options.c
SRCS+=	parser-attributes.c
SRCS+=	parser-braces.c
SRCS+=	parser-cpp.c
SRCS+=	parser-decl.c
SRCS+=	parser-expr.c
SRCS+=	parser-extern.c
SRCS+=	parser-func.c
SRCS+=	parser-stmt-asm.c
SRCS+=	parser-stmt-expr.c
SRCS+=	parser-stmt.c
SRCS+=	parser-type.c
SRCS+=	parser.c
SRCS+=	ruler.c
SRCS+=	simple-decl-proto.c
SRCS+=	simple-decl.c
SRCS+=	simple-stmt.c
SRCS+=	simple.c
SRCS+=	style.c
SRCS+=	token.c
SRCS+=	util.c
SRCS+=	vector.c

SRCS_knfmt+=	${SRCS}
SRCS_knfmt+=	knfmt.c
OBJS_knfmt=	${SRCS_knfmt:.c=.o}
DEPS_knfmt=	${SRCS_knfmt:.c=.d}
PROG_knfmt=	knfmt

SRCS_test+=	${SRCS}
SRCS_test+=	t.c
OBJS_test=	${SRCS_test:.c=.o}
DEPS_test=	${SRCS_test:.c=.d}
PROG_test=	t

SRCS_fuzz-style+=	${SRCS}
SRCS_fuzz-style+=	fuzz-style.c
OBJS_fuzz-style=	${SRCS_fuzz-style:.c=.o}
DEPS_fuzz-style=	${SRCS_fuzz-style:.c=.d}
PROG_fuzz-style=	fuzz-style

KNFMT+=	alloc.c
KNFMT+=	alloc.h
KNFMT+=	cdefs.h
KNFMT+=	clang.c
KNFMT+=	clang.h
KNFMT+=	comment.c
KNFMT+=	comment.h
KNFMT+=	compat-pledge.c
KNFMT+=	cpp-align.c
KNFMT+=	cpp-align.h
KNFMT+=	cpp-include.c
KNFMT+=	cpp-include.h
KNFMT+=	diff.c
KNFMT+=	diff.h
KNFMT+=	doc.c
KNFMT+=	doc.h
KNFMT+=	error.c
KNFMT+=	error.h
KNFMT+=	expr.c
KNFMT+=	expr.h
KNFMT+=	file.c
KNFMT+=	file.h
KNFMT+=	fuzz-style.c
KNFMT+=	knfmt.c
KNFMT+=	lexer.c
KNFMT+=	lexer.h
KNFMT+=	options.c
KNFMT+=	options.h
KNFMT+=	parser-attributes.c
KNFMT+=	parser-attributes.h
KNFMT+=	parser-braces.c
KNFMT+=	parser-braces.h
KNFMT+=	parser-cpp.c
KNFMT+=	parser-cpp.h
KNFMT+=	parser-decl.c
KNFMT+=	parser-decl.h
KNFMT+=	parser-expr.c
KNFMT+=	parser-expr.h
KNFMT+=	parser-extern.c
KNFMT+=	parser-extern.h
KNFMT+=	parser-func.c
KNFMT+=	parser-func.h
KNFMT+=	parser-priv.h
KNFMT+=	parser-stmt-asm.c
KNFMT+=	parser-stmt-asm.h
KNFMT+=	parser-stmt-expr.c
KNFMT+=	parser-stmt-expr.h
KNFMT+=	parser-stmt.c
KNFMT+=	parser-stmt.h
KNFMT+=	parser-type.c
KNFMT+=	parser-type.h
KNFMT+=	parser.c
KNFMT+=	parser.h
KNFMT+=	queue-fwd.h
KNFMT+=	ruler.c
KNFMT+=	ruler.h
KNFMT+=	simple-decl-proto.c
KNFMT+=	simple-decl-proto.h
KNFMT+=	simple-decl.c
KNFMT+=	simple-decl.h
KNFMT+=	simple-stmt.c
KNFMT+=	simple-stmt.h
KNFMT+=	simple.c
KNFMT+=	simple.h
KNFMT+=	style.c
KNFMT+=	style.h
KNFMT+=	t.c
KNFMT+=	token.c
KNFMT+=	token.h
KNFMT+=	util.c
KNFMT+=	util.h

CLANGTIDY+=	alloc.c
CLANGTIDY+=	alloc.h
CLANGTIDY+=	cdefs.h
CLANGTIDY+=	clang.c
CLANGTIDY+=	clang.h
CLANGTIDY+=	comment.c
CLANGTIDY+=	comment.h
CLANGTIDY+=	cpp-align.c
CLANGTIDY+=	cpp-align.h
CLANGTIDY+=	cpp-include.c
CLANGTIDY+=	cpp-include.h
CLANGTIDY+=	diff.c
CLANGTIDY+=	diff.h
CLANGTIDY+=	doc.c
CLANGTIDY+=	doc.h
CLANGTIDY+=	error.c
CLANGTIDY+=	error.h
CLANGTIDY+=	expr.c
CLANGTIDY+=	expr.h
CLANGTIDY+=	file.c
CLANGTIDY+=	file.h
CLANGTIDY+=	fuzz-style.c
CLANGTIDY+=	knfmt.c
CLANGTIDY+=	lexer.c
CLANGTIDY+=	lexer.h
CLANGTIDY+=	options.c
CLANGTIDY+=	options.h
CLANGTIDY+=	parser-attributes.c
CLANGTIDY+=	parser-attributes.h
CLANGTIDY+=	parser-braces.c
CLANGTIDY+=	parser-braces.h
CLANGTIDY+=	parser-cpp.c
CLANGTIDY+=	parser-cpp.h
CLANGTIDY+=	parser-decl.c
CLANGTIDY+=	parser-decl.h
CLANGTIDY+=	parser-expr.c
CLANGTIDY+=	parser-expr.h
CLANGTIDY+=	parser-extern.c
CLANGTIDY+=	parser-extern.h
CLANGTIDY+=	parser-func.c
CLANGTIDY+=	parser-func.h
CLANGTIDY+=	parser-priv.h
CLANGTIDY+=	parser-stmt-asm.c
CLANGTIDY+=	parser-stmt-asm.h
CLANGTIDY+=	parser-stmt-expr.c
CLANGTIDY+=	parser-stmt-expr.h
CLANGTIDY+=	parser-stmt.c
CLANGTIDY+=	parser-stmt.h
CLANGTIDY+=	parser-type.c
CLANGTIDY+=	parser-type.h
CLANGTIDY+=	parser.c
CLANGTIDY+=	parser.h
CLANGTIDY+=	queue-fwd.h
CLANGTIDY+=	ruler.c
CLANGTIDY+=	ruler.h
CLANGTIDY+=	simple-decl-proto.c
CLANGTIDY+=	simple-decl-proto.h
CLANGTIDY+=	simple-decl.c
CLANGTIDY+=	simple-decl.h
CLANGTIDY+=	simple-stmt.c
CLANGTIDY+=	simple-stmt.h
CLANGTIDY+=	simple.c
CLANGTIDY+=	simple.h
CLANGTIDY+=	style.c
CLANGTIDY+=	style.h
CLANGTIDY+=	t.c
CLANGTIDY+=	token.c
CLANGTIDY+=	token.h
CLANGTIDY+=	util.c
CLANGTIDY+=	util.h

CPPCHECK+=	alloc.c
CPPCHECK+=	clang.c
CPPCHECK+=	comment.c
CPPCHECK+=	cpp-align.c
CPPCHECK+=	cpp-include.c
CPPCHECK+=	diff.c
CPPCHECK+=	doc.c
CPPCHECK+=	error.c
CPPCHECK+=	expr.c
CPPCHECK+=	file.c
CPPCHECK+=	fuzz-style.c
CPPCHECK+=	knfmt.c
CPPCHECK+=	lexer.c
CPPCHECK+=	options.c
CPPCHECK+=	parser-attributes.c
CPPCHECK+=	parser-braces.c
CPPCHECK+=	parser-cpp.c
CPPCHECK+=	parser-decl.c
CPPCHECK+=	parser-expr.c
CPPCHECK+=	parser-extern.c
CPPCHECK+=	parser-func.c
CPPCHECK+=	parser-stmt-asm.c
CPPCHECK+=	parser-stmt-expr.c
CPPCHECK+=	parser-stmt.c
CPPCHECK+=	parser-type.c
CPPCHECK+=	parser.c
CPPCHECK+=	ruler.c
CPPCHECK+=	simple-decl-proto.c
CPPCHECK+=	simple-decl.c
CPPCHECK+=	simple-stmt.c
CPPCHECK+=	simple.c
CPPCHECK+=	style.c
CPPCHECK+=	t.c
CPPCHECK+=	token.c
CPPCHECK+=	util.c

CPPCHECKFLAGS+=	--quiet
CPPCHECKFLAGS+=	--enable=all
CPPCHECKFLAGS+=	--error-exitcode=1
CPPCHECKFLAGS+=	--max-configs=2
CPPCHECKFLAGS+=	--suppress-xml=cppcheck-suppressions.xml
CPPCHECKFLAGS+=	${CPPFLAGS}

SHLINT+=	configure
SHLINT+=	tests/cp.sh
SHLINT+=	tests/diff.sh
SHLINT+=	tests/enoent.sh
SHLINT+=	tests/git.sh
SHLINT+=	tests/knfmt.sh
SHLINT+=	tests/simple.sh

DISTFILES+=	CHANGELOG.md
DISTFILES+=	GNUmakefile
DISTFILES+=	LICENSE
DISTFILES+=	Makefile
DISTFILES+=	README.md
DISTFILES+=	alloc.c
DISTFILES+=	alloc.h
DISTFILES+=	cdefs.h
DISTFILES+=	clang.c
DISTFILES+=	clang.h
DISTFILES+=	comment.c
DISTFILES+=	comment.h
DISTFILES+=	compat-errc.c
DISTFILES+=	compat-pledge.c
DISTFILES+=	compat-queue.h
DISTFILES+=	compat-warnc.c
DISTFILES+=	configure
DISTFILES+=	cpp-align.c
DISTFILES+=	cpp-align.h
DISTFILES+=	cpp-include.c
DISTFILES+=	cpp-include.h
DISTFILES+=	diff.c
DISTFILES+=	diff.h
DISTFILES+=	doc.c
DISTFILES+=	doc.h
DISTFILES+=	error.c
DISTFILES+=	error.h
DISTFILES+=	expr.c
DISTFILES+=	expr.h
DISTFILES+=	file.c
DISTFILES+=	file.h
DISTFILES+=	fuzz-style.c
DISTFILES+=	knfmt.1
DISTFILES+=	knfmt.c
DISTFILES+=	lexer.c
DISTFILES+=	lexer.h
DISTFILES+=	libks/arithmetic.c
DISTFILES+=	libks/arithmetic.h
DISTFILES+=	libks/buffer.c
DISTFILES+=	libks/buffer.h
DISTFILES+=	libks/consistency.c
DISTFILES+=	libks/consistency.h
DISTFILES+=	libks/vector.c
DISTFILES+=	libks/vector.h
DISTFILES+=	options.c
DISTFILES+=	options.h
DISTFILES+=	parser-attributes.c
DISTFILES+=	parser-attributes.h
DISTFILES+=	parser-braces.c
DISTFILES+=	parser-braces.h
DISTFILES+=	parser-cpp.c
DISTFILES+=	parser-cpp.h
DISTFILES+=	parser-decl.c
DISTFILES+=	parser-decl.h
DISTFILES+=	parser-expr.c
DISTFILES+=	parser-expr.h
DISTFILES+=	parser-extern.c
DISTFILES+=	parser-extern.h
DISTFILES+=	parser-func.c
DISTFILES+=	parser-func.h
DISTFILES+=	parser-priv.h
DISTFILES+=	parser-stmt-asm.c
DISTFILES+=	parser-stmt-asm.h
DISTFILES+=	parser-stmt-expr.c
DISTFILES+=	parser-stmt-expr.h
DISTFILES+=	parser-stmt.c
DISTFILES+=	parser-stmt.h
DISTFILES+=	parser-type.c
DISTFILES+=	parser-type.h
DISTFILES+=	parser.c
DISTFILES+=	parser.h
DISTFILES+=	queue-fwd.h
DISTFILES+=	ruler.c
DISTFILES+=	ruler.h
DISTFILES+=	simple-decl-proto.c
DISTFILES+=	simple-decl-proto.h
DISTFILES+=	simple-decl.c
DISTFILES+=	simple-decl.h
DISTFILES+=	simple-stmt.c
DISTFILES+=	simple-stmt.h
DISTFILES+=	simple.c
DISTFILES+=	simple.h
DISTFILES+=	style.c
DISTFILES+=	style.h
DISTFILES+=	t.c
DISTFILES+=	tests/GNUmakefile
DISTFILES+=	tests/Makefile
DISTFILES+=	tests/bug-001.c
DISTFILES+=	tests/bug-001.ok
DISTFILES+=	tests/bug-002.c
DISTFILES+=	tests/bug-002.ok
DISTFILES+=	tests/cp.sh
DISTFILES+=	tests/diff-001.c
DISTFILES+=	tests/diff-001.ok
DISTFILES+=	tests/diff-001.patch
DISTFILES+=	tests/diff-002.c
DISTFILES+=	tests/diff-002.ok
DISTFILES+=	tests/diff-002.patch
DISTFILES+=	tests/diff-003.c
DISTFILES+=	tests/diff-003.ok
DISTFILES+=	tests/diff-003.patch
DISTFILES+=	tests/diff-004.c
DISTFILES+=	tests/diff-004.ok
DISTFILES+=	tests/diff-004.patch
DISTFILES+=	tests/diff-005.c
DISTFILES+=	tests/diff-005.ok
DISTFILES+=	tests/diff-005.patch
DISTFILES+=	tests/diff-006.c
DISTFILES+=	tests/diff-006.ok
DISTFILES+=	tests/diff-006.patch
DISTFILES+=	tests/diff-007.c
DISTFILES+=	tests/diff-007.ok
DISTFILES+=	tests/diff-007.patch
DISTFILES+=	tests/diff-008.c
DISTFILES+=	tests/diff-008.ok
DISTFILES+=	tests/diff-008.patch
DISTFILES+=	tests/diff-009.c
DISTFILES+=	tests/diff-009.ok
DISTFILES+=	tests/diff-009.patch
DISTFILES+=	tests/diff-010.c
DISTFILES+=	tests/diff-010.ok
DISTFILES+=	tests/diff-010.patch
DISTFILES+=	tests/diff-011.c
DISTFILES+=	tests/diff-011.ok
DISTFILES+=	tests/diff-011.patch
DISTFILES+=	tests/diff-012.c
DISTFILES+=	tests/diff-012.ok
DISTFILES+=	tests/diff-012.patch
DISTFILES+=	tests/diff-013.c
DISTFILES+=	tests/diff-013.ok
DISTFILES+=	tests/diff-013.patch
DISTFILES+=	tests/diff-014.c
DISTFILES+=	tests/diff-014.ok
DISTFILES+=	tests/diff-014.patch
DISTFILES+=	tests/diff-015.c
DISTFILES+=	tests/diff-015.ok
DISTFILES+=	tests/diff-015.patch
DISTFILES+=	tests/diff-016.c
DISTFILES+=	tests/diff-016.ok
DISTFILES+=	tests/diff-016.patch
DISTFILES+=	tests/diff-017.c
DISTFILES+=	tests/diff-017.ok
DISTFILES+=	tests/diff-017.patch
DISTFILES+=	tests/diff-018.c
DISTFILES+=	tests/diff-018.ok
DISTFILES+=	tests/diff-018.patch
DISTFILES+=	tests/diff-019.c
DISTFILES+=	tests/diff-019.ok
DISTFILES+=	tests/diff-019.patch
DISTFILES+=	tests/diff-020.c
DISTFILES+=	tests/diff-020.ok
DISTFILES+=	tests/diff-020.patch
DISTFILES+=	tests/diff-021.c
DISTFILES+=	tests/diff-021.ok
DISTFILES+=	tests/diff-021.patch
DISTFILES+=	tests/diff-022.c
DISTFILES+=	tests/diff-022.ok
DISTFILES+=	tests/diff-022.patch
DISTFILES+=	tests/diff-023.c
DISTFILES+=	tests/diff-023.ok
DISTFILES+=	tests/diff-023.patch
DISTFILES+=	tests/diff-024.c
DISTFILES+=	tests/diff-024.ok
DISTFILES+=	tests/diff-024.patch
DISTFILES+=	tests/diff-025.c
DISTFILES+=	tests/diff-025.ok
DISTFILES+=	tests/diff-025.patch
DISTFILES+=	tests/diff-026.c
DISTFILES+=	tests/diff-026.ok
DISTFILES+=	tests/diff-026.patch
DISTFILES+=	tests/diff-027.c
DISTFILES+=	tests/diff-027.ok
DISTFILES+=	tests/diff-027.patch
DISTFILES+=	tests/diff-028.c
DISTFILES+=	tests/diff-028.ok
DISTFILES+=	tests/diff-028.patch
DISTFILES+=	tests/diff-029.c
DISTFILES+=	tests/diff-029.ok
DISTFILES+=	tests/diff-029.patch
DISTFILES+=	tests/diff-030.c
DISTFILES+=	tests/diff-030.ok
DISTFILES+=	tests/diff-030.patch
DISTFILES+=	tests/diff-031.c
DISTFILES+=	tests/diff-031.ok
DISTFILES+=	tests/diff-031.patch
DISTFILES+=	tests/diff-032.c
DISTFILES+=	tests/diff-032.ok
DISTFILES+=	tests/diff-032.patch
DISTFILES+=	tests/diff-033.c
DISTFILES+=	tests/diff-033.ok
DISTFILES+=	tests/diff-033.patch
DISTFILES+=	tests/diff-simple-001.c
DISTFILES+=	tests/diff-simple-001.ok
DISTFILES+=	tests/diff-simple-001.patch
DISTFILES+=	tests/diff-simple-002.c
DISTFILES+=	tests/diff-simple-002.ok
DISTFILES+=	tests/diff-simple-002.patch
DISTFILES+=	tests/diff-simple-003.c
DISTFILES+=	tests/diff-simple-003.ok
DISTFILES+=	tests/diff-simple-003.patch
DISTFILES+=	tests/diff-simple-004.c
DISTFILES+=	tests/diff-simple-004.ok
DISTFILES+=	tests/diff-simple-004.patch
DISTFILES+=	tests/diff-style-001.c
DISTFILES+=	tests/diff-style-001.ok
DISTFILES+=	tests/diff-style-001.patch
DISTFILES+=	tests/diff-style-002.c
DISTFILES+=	tests/diff-style-002.ok
DISTFILES+=	tests/diff-style-002.patch
DISTFILES+=	tests/diff.sh
DISTFILES+=	tests/enoent.sh
DISTFILES+=	tests/error-001.c
DISTFILES+=	tests/error-002.c
DISTFILES+=	tests/error-003.c
DISTFILES+=	tests/error-004.c
DISTFILES+=	tests/error-005.c
DISTFILES+=	tests/error-006.c
DISTFILES+=	tests/error-007.c
DISTFILES+=	tests/error-008.c
DISTFILES+=	tests/error-009.c
DISTFILES+=	tests/error-010.c
DISTFILES+=	tests/error-011.c
DISTFILES+=	tests/error-012.c
DISTFILES+=	tests/error-013.c
DISTFILES+=	tests/error-014.c
DISTFILES+=	tests/error-015.c
DISTFILES+=	tests/error-016.c
DISTFILES+=	tests/error-017.c
DISTFILES+=	tests/error-018.c
DISTFILES+=	tests/error-019.c
DISTFILES+=	tests/error-020.c
DISTFILES+=	tests/error-021.c
DISTFILES+=	tests/error-022.c
DISTFILES+=	tests/error-023.c
DISTFILES+=	tests/error-024.c
DISTFILES+=	tests/error-025.c
DISTFILES+=	tests/error-026.c
DISTFILES+=	tests/error-027.c
DISTFILES+=	tests/error-028.c
DISTFILES+=	tests/error-029.c
DISTFILES+=	tests/error-030.c
DISTFILES+=	tests/error-031.c
DISTFILES+=	tests/error-032.c
DISTFILES+=	tests/error-033.c
DISTFILES+=	tests/error-034.c
DISTFILES+=	tests/error-035.c
DISTFILES+=	tests/error-036.c
DISTFILES+=	tests/error-037.c
DISTFILES+=	tests/error-038.c
DISTFILES+=	tests/error-039.c
DISTFILES+=	tests/error-040.c
DISTFILES+=	tests/error-040.ok
DISTFILES+=	tests/error-041.c
DISTFILES+=	tests/error-042.c
DISTFILES+=	tests/error-043.c
DISTFILES+=	tests/error-044.c
DISTFILES+=	tests/error-045.c
DISTFILES+=	tests/error-046.c
DISTFILES+=	tests/error-047.c
DISTFILES+=	tests/error-048.c
DISTFILES+=	tests/error-049.c
DISTFILES+=	tests/error-050.c
DISTFILES+=	tests/error-051.c
DISTFILES+=	tests/error-052.c
DISTFILES+=	tests/error-053.c
DISTFILES+=	tests/error-054.c
DISTFILES+=	tests/error-055.c
DISTFILES+=	tests/error-056.c
DISTFILES+=	tests/error-057.c
DISTFILES+=	tests/error-058.c
DISTFILES+=	tests/git.sh
DISTFILES+=	tests/inplace-001.c
DISTFILES+=	tests/knfmt.sh
DISTFILES+=	tests/simple-001.c
DISTFILES+=	tests/simple-001.ok
DISTFILES+=	tests/simple-002.c
DISTFILES+=	tests/simple-002.ok
DISTFILES+=	tests/simple-003.c
DISTFILES+=	tests/simple-003.ok
DISTFILES+=	tests/simple-004.c
DISTFILES+=	tests/simple-004.ok
DISTFILES+=	tests/simple-005.c
DISTFILES+=	tests/simple-005.ok
DISTFILES+=	tests/simple-006.c
DISTFILES+=	tests/simple-006.ok
DISTFILES+=	tests/simple-007.c
DISTFILES+=	tests/simple-007.ok
DISTFILES+=	tests/simple-008.c
DISTFILES+=	tests/simple-009.c
DISTFILES+=	tests/simple-009.ok
DISTFILES+=	tests/simple-010.c
DISTFILES+=	tests/simple-010.ok
DISTFILES+=	tests/simple-011.c
DISTFILES+=	tests/simple-012.c
DISTFILES+=	tests/simple-012.ok
DISTFILES+=	tests/simple-013.c
DISTFILES+=	tests/simple-013.ok
DISTFILES+=	tests/simple-014.c
DISTFILES+=	tests/simple-014.ok
DISTFILES+=	tests/simple-015.c
DISTFILES+=	tests/simple-015.ok
DISTFILES+=	tests/simple-016.c
DISTFILES+=	tests/simple-016.ok
DISTFILES+=	tests/simple-017.c
DISTFILES+=	tests/simple-018.c
DISTFILES+=	tests/simple-018.ok
DISTFILES+=	tests/simple-019.c
DISTFILES+=	tests/simple-019.ok
DISTFILES+=	tests/simple-020.c
DISTFILES+=	tests/simple-020.ok
DISTFILES+=	tests/simple-021.c
DISTFILES+=	tests/simple-021.ok
DISTFILES+=	tests/simple-022.c
DISTFILES+=	tests/simple-022.ok
DISTFILES+=	tests/simple-023.c
DISTFILES+=	tests/simple-023.ok
DISTFILES+=	tests/simple-024.c
DISTFILES+=	tests/simple-025.c
DISTFILES+=	tests/simple-025.ok
DISTFILES+=	tests/simple-026.c
DISTFILES+=	tests/simple-026.ok
DISTFILES+=	tests/simple-027.c
DISTFILES+=	tests/simple-027.ok
DISTFILES+=	tests/simple-028.c
DISTFILES+=	tests/simple-028.ok
DISTFILES+=	tests/simple-029.c
DISTFILES+=	tests/simple-030.c
DISTFILES+=	tests/simple-031.c
DISTFILES+=	tests/simple-032.c
DISTFILES+=	tests/simple-032.ok
DISTFILES+=	tests/simple-033.c
DISTFILES+=	tests/simple-033.ok
DISTFILES+=	tests/simple-034.c
DISTFILES+=	tests/simple-034.ok
DISTFILES+=	tests/simple-035.c
DISTFILES+=	tests/simple-035.ok
DISTFILES+=	tests/simple-036.c
DISTFILES+=	tests/simple-036.ok
DISTFILES+=	tests/simple-037.ok
DISTFILES+=	tests/simple-038.c
DISTFILES+=	tests/simple-038.ok
DISTFILES+=	tests/simple-039.c
DISTFILES+=	tests/simple-039.ok
DISTFILES+=	tests/simple-040.c
DISTFILES+=	tests/simple-040.ok
DISTFILES+=	tests/simple-041.c
DISTFILES+=	tests/simple-041.ok
DISTFILES+=	tests/simple-042.c
DISTFILES+=	tests/simple-042.ok
DISTFILES+=	tests/simple.sh
DISTFILES+=	tests/style-AlignAfterOpenBracket-001.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-002.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-003.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-004.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-005.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-006.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-007.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-008.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-009.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-010.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-011.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-012.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-012.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-013.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-014.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-015.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-016.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-017.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-018.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-019.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-020.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-021.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-022.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-022.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-023.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-023.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-024.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-025.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-026.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-027.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-028.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-029.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-030.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-031.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-031.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-032.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-033.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-033.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-034.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-034.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-035.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-035.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-036.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-036.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-037.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-037.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-038.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-038.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-039.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-040.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-041.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-042.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-042.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-043.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-044.c
DISTFILES+=	tests/style-AlignAfterOpenBracket-044.ok
DISTFILES+=	tests/style-AlignAfterOpenBracket-045.c
DISTFILES+=	tests/style-AlignEscapedNewlines-001.c
DISTFILES+=	tests/style-AlignEscapedNewlines-002.c
DISTFILES+=	tests/style-AlignEscapedNewlines-002.ok
DISTFILES+=	tests/style-AlignEscapedNewlines-003.c
DISTFILES+=	tests/style-AlignEscapedNewlines-003.ok
DISTFILES+=	tests/style-AlignEscapedNewlines-004.c
DISTFILES+=	tests/style-AlignEscapedNewlines-004.ok
DISTFILES+=	tests/style-AlignOperands-001.c
DISTFILES+=	tests/style-AlignOperands-001.ok
DISTFILES+=	tests/style-AlignOperands-002.c
DISTFILES+=	tests/style-AlignOperands-003.c
DISTFILES+=	tests/style-AlignOperands-004.c
DISTFILES+=	tests/style-AlignOperands-004.ok
DISTFILES+=	tests/style-AlignOperands-005.c
DISTFILES+=	tests/style-AlignOperands-006.c
DISTFILES+=	tests/style-AlignOperands-007.c
DISTFILES+=	tests/style-AlignOperands-008.c
DISTFILES+=	tests/style-AlignOperands-009.c
DISTFILES+=	tests/style-AlignOperands-010.c
DISTFILES+=	tests/style-AlignOperands-011.c
DISTFILES+=	tests/style-AlignOperands-012.c
DISTFILES+=	tests/style-AlignOperands-012.ok
DISTFILES+=	tests/style-AlwaysBreakAfterReturnType-001.c
DISTFILES+=	tests/style-AlwaysBreakAfterReturnType-001.ok
DISTFILES+=	tests/style-AlwaysBreakAfterReturnType-002.c
DISTFILES+=	tests/style-BitFieldColonSpacing-001.c
DISTFILES+=	tests/style-BitFieldColonSpacing-002.c
DISTFILES+=	tests/style-BitFieldColonSpacing-003.c
DISTFILES+=	tests/style-BraceWrapping-001.c
DISTFILES+=	tests/style-BraceWrapping-001.ok
DISTFILES+=	tests/style-BraceWrapping-002.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-001.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-001.ok
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-002.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-003.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-004.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-005.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-006.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-006.ok
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-007.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-008.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-008.ok
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-009.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-009.ok
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-010.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-010.ok
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-011.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-012.c
DISTFILES+=	tests/style-BreakBeforeBinaryOperators-012.ok
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-001.c
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-001.ok
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-002.c
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-002.ok
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-003.c
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-004.c
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-005.c
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-006.c
DISTFILES+=	tests/style-BreakBeforeTernaryOperators-006.ok
DISTFILES+=	tests/style-ColumnLimit-001.c
DISTFILES+=	tests/style-ColumnLimit-002.c
DISTFILES+=	tests/style-ContinuationIndentWidth-001.c
DISTFILES+=	tests/style-ContinuationIndentWidth-001.ok
DISTFILES+=	tests/style-IncludeCategories-001.c
DISTFILES+=	tests/style-IndentWidth-001.c
DISTFILES+=	tests/style-IndentWidth-001.ok
DISTFILES+=	tests/style-SortIncludes-001.c
DISTFILES+=	tests/style-SortIncludes-001.ok
DISTFILES+=	tests/style-UseTab-001.c
DISTFILES+=	tests/style-UseTab-002.c
DISTFILES+=	tests/style-bug-BraceWrapping-001.c
DISTFILES+=	tests/style-bug-BraceWrapping-001.ok
DISTFILES+=	tests/style-error-001.c
DISTFILES+=	tests/style-error-001.ok
DISTFILES+=	tests/style-error-002.c
DISTFILES+=	tests/style-error-002.ok
DISTFILES+=	tests/style-error-003.c
DISTFILES+=	tests/style-error-003.ok
DISTFILES+=	tests/style-error-004.c
DISTFILES+=	tests/style-error-004.ok
DISTFILES+=	tests/style-error-005.c
DISTFILES+=	tests/style-error-005.ok
DISTFILES+=	tests/style-error-006.c
DISTFILES+=	tests/style-error-006.ok
DISTFILES+=	tests/style-error-007.c
DISTFILES+=	tests/style-error-007.ok
DISTFILES+=	tests/style-error-008.c
DISTFILES+=	tests/style-error-008.ok
DISTFILES+=	tests/style-simple-AlignOperands-001.c
DISTFILES+=	tests/style-simple-AlignOperands-001.ok
DISTFILES+=	tests/style-simple-AlignOperands-002.c
DISTFILES+=	tests/style-simple-AlignOperands-002.ok
DISTFILES+=	tests/trace-001.c
DISTFILES+=	tests/valid-001.c
DISTFILES+=	tests/valid-002.c
DISTFILES+=	tests/valid-003.c
DISTFILES+=	tests/valid-004.c
DISTFILES+=	tests/valid-005.c
DISTFILES+=	tests/valid-006.c
DISTFILES+=	tests/valid-007.c
DISTFILES+=	tests/valid-008.c
DISTFILES+=	tests/valid-009.c
DISTFILES+=	tests/valid-010.c
DISTFILES+=	tests/valid-011.c
DISTFILES+=	tests/valid-012.c
DISTFILES+=	tests/valid-013.c
DISTFILES+=	tests/valid-014.c
DISTFILES+=	tests/valid-015.c
DISTFILES+=	tests/valid-016.c
DISTFILES+=	tests/valid-017.c
DISTFILES+=	tests/valid-018.c
DISTFILES+=	tests/valid-019.c
DISTFILES+=	tests/valid-020.c
DISTFILES+=	tests/valid-021.c
DISTFILES+=	tests/valid-022.c
DISTFILES+=	tests/valid-023.c
DISTFILES+=	tests/valid-024.c
DISTFILES+=	tests/valid-025.c
DISTFILES+=	tests/valid-026.c
DISTFILES+=	tests/valid-027.c
DISTFILES+=	tests/valid-028.c
DISTFILES+=	tests/valid-029.c
DISTFILES+=	tests/valid-030.c
DISTFILES+=	tests/valid-031.c
DISTFILES+=	tests/valid-032.c
DISTFILES+=	tests/valid-033.c
DISTFILES+=	tests/valid-034.c
DISTFILES+=	tests/valid-035.c
DISTFILES+=	tests/valid-036.c
DISTFILES+=	tests/valid-037.c
DISTFILES+=	tests/valid-038.c
DISTFILES+=	tests/valid-039.c
DISTFILES+=	tests/valid-039.ok
DISTFILES+=	tests/valid-040.c
DISTFILES+=	tests/valid-041.c
DISTFILES+=	tests/valid-042.c
DISTFILES+=	tests/valid-043.c
DISTFILES+=	tests/valid-044.c
DISTFILES+=	tests/valid-045.c
DISTFILES+=	tests/valid-046.c
DISTFILES+=	tests/valid-047.c
DISTFILES+=	tests/valid-048.c
DISTFILES+=	tests/valid-049.c
DISTFILES+=	tests/valid-050.c
DISTFILES+=	tests/valid-051.c
DISTFILES+=	tests/valid-052.c
DISTFILES+=	tests/valid-053.c
DISTFILES+=	tests/valid-054.c
DISTFILES+=	tests/valid-055.c
DISTFILES+=	tests/valid-056.c
DISTFILES+=	tests/valid-057.c
DISTFILES+=	tests/valid-058.c
DISTFILES+=	tests/valid-059.c
DISTFILES+=	tests/valid-060.c
DISTFILES+=	tests/valid-061.c
DISTFILES+=	tests/valid-062.c
DISTFILES+=	tests/valid-063.c
DISTFILES+=	tests/valid-064.c
DISTFILES+=	tests/valid-065.c
DISTFILES+=	tests/valid-066.c
DISTFILES+=	tests/valid-067.c
DISTFILES+=	tests/valid-068.c
DISTFILES+=	tests/valid-069.c
DISTFILES+=	tests/valid-069.ok
DISTFILES+=	tests/valid-070.c
DISTFILES+=	tests/valid-070.ok
DISTFILES+=	tests/valid-071.c
DISTFILES+=	tests/valid-072.c
DISTFILES+=	tests/valid-073.c
DISTFILES+=	tests/valid-074.c
DISTFILES+=	tests/valid-075.c
DISTFILES+=	tests/valid-076.c
DISTFILES+=	tests/valid-077.c
DISTFILES+=	tests/valid-078.c
DISTFILES+=	tests/valid-079.c
DISTFILES+=	tests/valid-080.c
DISTFILES+=	tests/valid-080.ok
DISTFILES+=	tests/valid-081.c
DISTFILES+=	tests/valid-082.c
DISTFILES+=	tests/valid-083.c
DISTFILES+=	tests/valid-084.c
DISTFILES+=	tests/valid-085.c
DISTFILES+=	tests/valid-086.c
DISTFILES+=	tests/valid-087.c
DISTFILES+=	tests/valid-088.c
DISTFILES+=	tests/valid-089.c
DISTFILES+=	tests/valid-089.ok
DISTFILES+=	tests/valid-090.c
DISTFILES+=	tests/valid-091.c
DISTFILES+=	tests/valid-091.ok
DISTFILES+=	tests/valid-092.c
DISTFILES+=	tests/valid-093.c
DISTFILES+=	tests/valid-094.c
DISTFILES+=	tests/valid-095.c
DISTFILES+=	tests/valid-096.c
DISTFILES+=	tests/valid-097.c
DISTFILES+=	tests/valid-098.c
DISTFILES+=	tests/valid-099.c
DISTFILES+=	tests/valid-100.c
DISTFILES+=	tests/valid-101.c
DISTFILES+=	tests/valid-102.c
DISTFILES+=	tests/valid-103.c
DISTFILES+=	tests/valid-104.c
DISTFILES+=	tests/valid-105.c
DISTFILES+=	tests/valid-106.c
DISTFILES+=	tests/valid-107.c
DISTFILES+=	tests/valid-108.c
DISTFILES+=	tests/valid-109.c
DISTFILES+=	tests/valid-110.c
DISTFILES+=	tests/valid-111.c
DISTFILES+=	tests/valid-112.c
DISTFILES+=	tests/valid-113.c
DISTFILES+=	tests/valid-114.c
DISTFILES+=	tests/valid-115.c
DISTFILES+=	tests/valid-116.c
DISTFILES+=	tests/valid-117.c
DISTFILES+=	tests/valid-118.c
DISTFILES+=	tests/valid-119.c
DISTFILES+=	tests/valid-120.c
DISTFILES+=	tests/valid-121.c
DISTFILES+=	tests/valid-122.c
DISTFILES+=	tests/valid-122.ok
DISTFILES+=	tests/valid-123.c
DISTFILES+=	tests/valid-124.c
DISTFILES+=	tests/valid-125.c
DISTFILES+=	tests/valid-126.c
DISTFILES+=	tests/valid-127.c
DISTFILES+=	tests/valid-128.c
DISTFILES+=	tests/valid-129.c
DISTFILES+=	tests/valid-130.c
DISTFILES+=	tests/valid-131.c
DISTFILES+=	tests/valid-132.c
DISTFILES+=	tests/valid-133.c
DISTFILES+=	tests/valid-134.c
DISTFILES+=	tests/valid-135.c
DISTFILES+=	tests/valid-136.c
DISTFILES+=	tests/valid-136.ok
DISTFILES+=	tests/valid-137.c
DISTFILES+=	tests/valid-137.ok
DISTFILES+=	tests/valid-138.c
DISTFILES+=	tests/valid-139.c
DISTFILES+=	tests/valid-140.c
DISTFILES+=	tests/valid-141.c
DISTFILES+=	tests/valid-142.c
DISTFILES+=	tests/valid-142.ok
DISTFILES+=	tests/valid-143.c
DISTFILES+=	tests/valid-144.c
DISTFILES+=	tests/valid-145.c
DISTFILES+=	tests/valid-146.c
DISTFILES+=	tests/valid-147.c
DISTFILES+=	tests/valid-148.c
DISTFILES+=	tests/valid-149.c
DISTFILES+=	tests/valid-150.c
DISTFILES+=	tests/valid-150.ok
DISTFILES+=	tests/valid-151.c
DISTFILES+=	tests/valid-152.c
DISTFILES+=	tests/valid-153.c
DISTFILES+=	tests/valid-153.ok
DISTFILES+=	tests/valid-154.c
DISTFILES+=	tests/valid-155.c
DISTFILES+=	tests/valid-155.ok
DISTFILES+=	tests/valid-156.c
DISTFILES+=	tests/valid-156.ok
DISTFILES+=	tests/valid-157.c
DISTFILES+=	tests/valid-157.ok
DISTFILES+=	tests/valid-158.c
DISTFILES+=	tests/valid-158.ok
DISTFILES+=	tests/valid-159.c
DISTFILES+=	tests/valid-160.c
DISTFILES+=	tests/valid-161.c
DISTFILES+=	tests/valid-162.c
DISTFILES+=	tests/valid-163.c
DISTFILES+=	tests/valid-164.c
DISTFILES+=	tests/valid-165.c
DISTFILES+=	tests/valid-166.c
DISTFILES+=	tests/valid-167.c
DISTFILES+=	tests/valid-168.c
DISTFILES+=	tests/valid-169.c
DISTFILES+=	tests/valid-170.c
DISTFILES+=	tests/valid-170.ok
DISTFILES+=	tests/valid-171.c
DISTFILES+=	tests/valid-172.c
DISTFILES+=	tests/valid-173.c
DISTFILES+=	tests/valid-174.c
DISTFILES+=	tests/valid-175.c
DISTFILES+=	tests/valid-176.c
DISTFILES+=	tests/valid-177.c
DISTFILES+=	tests/valid-178.c
DISTFILES+=	tests/valid-179.c
DISTFILES+=	tests/valid-180.c
DISTFILES+=	tests/valid-181.c
DISTFILES+=	tests/valid-181.ok
DISTFILES+=	tests/valid-182.c
DISTFILES+=	tests/valid-183.c
DISTFILES+=	tests/valid-184.c
DISTFILES+=	tests/valid-185.c
DISTFILES+=	tests/valid-186.c
DISTFILES+=	tests/valid-187.c
DISTFILES+=	tests/valid-187.ok
DISTFILES+=	tests/valid-188.c
DISTFILES+=	tests/valid-189.c
DISTFILES+=	tests/valid-189.ok
DISTFILES+=	tests/valid-190.c
DISTFILES+=	tests/valid-190.ok
DISTFILES+=	tests/valid-191.c
DISTFILES+=	tests/valid-191.ok
DISTFILES+=	tests/valid-192.c
DISTFILES+=	tests/valid-193.c
DISTFILES+=	tests/valid-194.c
DISTFILES+=	tests/valid-194.ok
DISTFILES+=	tests/valid-195.c
DISTFILES+=	tests/valid-195.ok
DISTFILES+=	tests/valid-196.c
DISTFILES+=	tests/valid-196.ok
DISTFILES+=	tests/valid-197.c
DISTFILES+=	tests/valid-198.c
DISTFILES+=	tests/valid-198.ok
DISTFILES+=	tests/valid-199.c
DISTFILES+=	tests/valid-199.ok
DISTFILES+=	tests/valid-200.c
DISTFILES+=	tests/valid-201.c
DISTFILES+=	tests/valid-202.c
DISTFILES+=	tests/valid-203.c
DISTFILES+=	tests/valid-204.c
DISTFILES+=	tests/valid-205.c
DISTFILES+=	tests/valid-206.c
DISTFILES+=	tests/valid-207.c
DISTFILES+=	tests/valid-208.c
DISTFILES+=	tests/valid-208.ok
DISTFILES+=	tests/valid-209.c
DISTFILES+=	tests/valid-209.ok
DISTFILES+=	tests/valid-210.c
DISTFILES+=	tests/valid-210.ok
DISTFILES+=	tests/valid-211.c
DISTFILES+=	tests/valid-212.c
DISTFILES+=	tests/valid-212.ok
DISTFILES+=	tests/valid-213.c
DISTFILES+=	tests/valid-213.ok
DISTFILES+=	tests/valid-214.c
DISTFILES+=	tests/valid-215.c
DISTFILES+=	tests/valid-216.c
DISTFILES+=	tests/valid-217.c
DISTFILES+=	tests/valid-218.c
DISTFILES+=	tests/valid-219.c
DISTFILES+=	tests/valid-220.c
DISTFILES+=	tests/valid-221.c
DISTFILES+=	tests/valid-222.c
DISTFILES+=	tests/valid-222.ok
DISTFILES+=	tests/valid-223.c
DISTFILES+=	tests/valid-224.c
DISTFILES+=	tests/valid-224.ok
DISTFILES+=	tests/valid-225.c
DISTFILES+=	tests/valid-225.ok
DISTFILES+=	tests/valid-226.c
DISTFILES+=	tests/valid-227.c
DISTFILES+=	tests/valid-228.c
DISTFILES+=	tests/valid-229.c
DISTFILES+=	tests/valid-230.c
DISTFILES+=	tests/valid-231.c
DISTFILES+=	tests/valid-231.ok
DISTFILES+=	tests/valid-232.c
DISTFILES+=	tests/valid-232.ok
DISTFILES+=	tests/valid-233.c
DISTFILES+=	tests/valid-233.ok
DISTFILES+=	tests/valid-234.c
DISTFILES+=	tests/valid-234.ok
DISTFILES+=	tests/valid-235.c
DISTFILES+=	tests/valid-235.ok
DISTFILES+=	tests/valid-236.c
DISTFILES+=	tests/valid-236.ok
DISTFILES+=	tests/valid-237.c
DISTFILES+=	tests/valid-238.c
DISTFILES+=	tests/valid-239.c
DISTFILES+=	tests/valid-239.ok
DISTFILES+=	tests/valid-240.c
DISTFILES+=	tests/valid-241.c
DISTFILES+=	tests/valid-242.c
DISTFILES+=	tests/valid-243.c
DISTFILES+=	tests/valid-243.ok
DISTFILES+=	tests/valid-244.c
DISTFILES+=	tests/valid-244.ok
DISTFILES+=	tests/valid-245.c
DISTFILES+=	tests/valid-246.c
DISTFILES+=	tests/valid-246.ok
DISTFILES+=	tests/valid-247.c
DISTFILES+=	tests/valid-248.c
DISTFILES+=	tests/valid-248.ok
DISTFILES+=	tests/valid-249.c
DISTFILES+=	tests/valid-249.ok
DISTFILES+=	tests/valid-250.c
DISTFILES+=	tests/valid-251.c
DISTFILES+=	tests/valid-252.c
DISTFILES+=	tests/valid-253.c
DISTFILES+=	tests/valid-254.c
DISTFILES+=	tests/valid-255.c
DISTFILES+=	tests/valid-256.c
DISTFILES+=	tests/valid-257.c
DISTFILES+=	tests/valid-258.c
DISTFILES+=	tests/valid-259.c
DISTFILES+=	tests/valid-260.c
DISTFILES+=	tests/valid-261.c
DISTFILES+=	tests/valid-261.ok
DISTFILES+=	tests/valid-262.c
DISTFILES+=	tests/valid-263.c
DISTFILES+=	tests/valid-264.c
DISTFILES+=	tests/valid-264.ok
DISTFILES+=	tests/valid-265.c
DISTFILES+=	tests/valid-265.ok
DISTFILES+=	tests/valid-266.c
DISTFILES+=	tests/valid-266.ok
DISTFILES+=	tests/valid-267.c
DISTFILES+=	tests/valid-268.c
DISTFILES+=	tests/valid-268.ok
DISTFILES+=	tests/valid-269.c
DISTFILES+=	tests/valid-270.c
DISTFILES+=	tests/valid-270.ok
DISTFILES+=	tests/valid-271.c
DISTFILES+=	tests/valid-271.ok
DISTFILES+=	tests/valid-272.c
DISTFILES+=	tests/valid-272.ok
DISTFILES+=	tests/valid-273.c
DISTFILES+=	tests/valid-274.c
DISTFILES+=	tests/valid-275.c
DISTFILES+=	tests/valid-276.c
DISTFILES+=	tests/valid-276.ok
DISTFILES+=	tests/valid-277.c
DISTFILES+=	tests/valid-278.c
DISTFILES+=	tests/valid-279.c
DISTFILES+=	tests/valid-279.ok
DISTFILES+=	tests/valid-280.c
DISTFILES+=	tests/valid-280.ok
DISTFILES+=	tests/valid-281.c
DISTFILES+=	tests/valid-281.ok
DISTFILES+=	tests/valid-282.c
DISTFILES+=	tests/valid-282.ok
DISTFILES+=	tests/valid-283.c
DISTFILES+=	tests/valid-283.ok
DISTFILES+=	tests/valid-284.c
DISTFILES+=	tests/valid-285.c
DISTFILES+=	tests/valid-286.c
DISTFILES+=	tests/valid-287.c
DISTFILES+=	tests/valid-287.ok
DISTFILES+=	tests/valid-288.c
DISTFILES+=	tests/valid-289.c
DISTFILES+=	tests/valid-289.ok
DISTFILES+=	tests/valid-290.c
DISTFILES+=	tests/valid-291.c
DISTFILES+=	tests/valid-292.c
DISTFILES+=	tests/valid-293.c
DISTFILES+=	tests/valid-294.c
DISTFILES+=	tests/valid-295.c
DISTFILES+=	tests/valid-296.c
DISTFILES+=	tests/valid-296.ok
DISTFILES+=	tests/valid-297.c
DISTFILES+=	tests/valid-297.ok
DISTFILES+=	tests/valid-298.c
DISTFILES+=	tests/valid-299.c
DISTFILES+=	tests/valid-299.ok
DISTFILES+=	tests/valid-300.c
DISTFILES+=	tests/valid-301.c
DISTFILES+=	tests/valid-301.ok
DISTFILES+=	tests/valid-302.c
DISTFILES+=	tests/valid-303.c
DISTFILES+=	tests/valid-303.ok
DISTFILES+=	tests/valid-304.c
DISTFILES+=	tests/valid-304.ok
DISTFILES+=	tests/valid-305.c
DISTFILES+=	tests/valid-305.ok
DISTFILES+=	tests/valid-306.c
DISTFILES+=	tests/valid-307.c
DISTFILES+=	tests/valid-307.ok
DISTFILES+=	tests/valid-308.c
DISTFILES+=	tests/valid-308.ok
DISTFILES+=	tests/valid-309.c
DISTFILES+=	tests/valid-309.ok
DISTFILES+=	tests/valid-310.c
DISTFILES+=	tests/valid-310.ok
DISTFILES+=	tests/valid-311.c
DISTFILES+=	tests/valid-312.c
DISTFILES+=	tests/valid-313.c
DISTFILES+=	tests/valid-314.c
DISTFILES+=	tests/valid-315.c
DISTFILES+=	tests/valid-316.c
DISTFILES+=	tests/valid-317.c
DISTFILES+=	tests/valid-318.c
DISTFILES+=	tests/valid-318.ok
DISTFILES+=	tests/valid-319.c
DISTFILES+=	tests/valid-319.ok
DISTFILES+=	tests/valid-320.c
DISTFILES+=	tests/valid-320.ok
DISTFILES+=	tests/valid-321.c
DISTFILES+=	tests/valid-321.ok
DISTFILES+=	tests/valid-322.c
DISTFILES+=	tests/valid-323.c
DISTFILES+=	tests/valid-323.ok
DISTFILES+=	tests/valid-324.c
DISTFILES+=	tests/valid-324.ok
DISTFILES+=	tests/valid-325.c
DISTFILES+=	tests/valid-325.ok
DISTFILES+=	tests/valid-326.c
DISTFILES+=	tests/valid-326.ok
DISTFILES+=	tests/valid-327.c
DISTFILES+=	tests/valid-327.ok
DISTFILES+=	tests/valid-328.c
DISTFILES+=	tests/valid-328.ok
DISTFILES+=	tests/valid-329.c
DISTFILES+=	tests/valid-330.c
DISTFILES+=	tests/valid-331.c
DISTFILES+=	tests/valid-332.c
DISTFILES+=	tests/valid-332.ok
DISTFILES+=	tests/valid-333.c
DISTFILES+=	tests/valid-334.c
DISTFILES+=	tests/valid-335.c
DISTFILES+=	tests/valid-335.ok
DISTFILES+=	tests/valid-336.c
DISTFILES+=	tests/valid-337.c
DISTFILES+=	tests/valid-337.ok
DISTFILES+=	tests/valid-338.c
DISTFILES+=	tests/valid-338.ok
DISTFILES+=	tests/valid-339.c
DISTFILES+=	tests/valid-339.ok
DISTFILES+=	tests/valid-340.c
DISTFILES+=	tests/valid-341.c
DISTFILES+=	token.c
DISTFILES+=	token.h
DISTFILES+=	util.c
DISTFILES+=	util.h

SHELLCHECKFLAGS+=	-f gcc
SHELLCHECKFLAGS+=	-s ksh
SHELLCHECKFLAGS+=	-o add-default-case
SHELLCHECKFLAGS+=	-o avoid-nullary-conditions
SHELLCHECKFLAGS+=	-o quote-safe-variables

all: ${PROG_knfmt}

${PROG_knfmt}: ${OBJS_knfmt}
	${CC} ${DEBUG} -o ${PROG_knfmt} ${OBJS_knfmt} ${LDFLAGS}

${PROG_test}: ${OBJS_test}
	${CC} ${DEBUG} -o ${PROG_test} ${OBJS_test} ${LDFLAGS}

clean:
	rm -f ${DEPS_knfmt} ${OBJS_knfmt} ${PROG_knfmt} \
		${DEPS_test} ${OBJS_test} ${PROG_test} \
		${DEPS_fuzz-style} ${OBJS_fuzz-style} ${PROG_fuzz-style}
.PHONY: clean

cleandir: clean
	cd ${.CURDIR} && rm -f config.h config.log config.mk
.PHONY: cleandir

dist:
	set -e; \
	d=knfmt-${VERSION}; \
	mkdir $$d; \
	for f in ${DISTFILES}; do \
		mkdir -p $$d/`dirname $$f`; \
		cp -p ${.CURDIR}/$$f $$d/$$f; \
	done; \
	find $$d -type d -exec touch -r ${.CURDIR}/Makefile {} \;; \
	tar czvf ${.CURDIR}/$$d.tar.gz $$d; \
	(cd ${.CURDIR}; sha256 $$d.tar.gz >$$d.sha256); \
	rm -r $$d
.PHONY: dist

format: ${PROG_knfmt}
	cd ${.CURDIR} && ${.OBJDIR}/${PROG_knfmt} -is ${KNFMT}
.PHONY: format

fuzz: ${PROG_fuzz-style}

${PROG_fuzz-style}: ${OBJS_fuzz-style}
	${CC} ${DEBUG} -o ${PROG_fuzz-style} ${OBJS_fuzz-style} ${LDFLAGS}

install: all
	@mkdir -p ${DESTDIR}${BINDIR}
	${INSTALL} ${PROG_knfmt} ${DESTDIR}${BINDIR}
	@mkdir -p ${DESTDIR}${MANDIR}/man1
	${INSTALL_MAN} ${.CURDIR}/knfmt.1 ${DESTDIR}${MANDIR}/man1
.PHONY: install

lint: ${PROG_knfmt}
	cd ${.CURDIR} && ${.OBJDIR}/${PROG_knfmt} -ds ${KNFMT}
	cd ${.CURDIR} && mandoc -Tlint -Wstyle knfmt.1
.PHONY: lint

lint-clang-tidy:
	cd ${.CURDIR} && echo ${CLANGTIDY} | xargs printf '%s\n' | \
		xargs -I{} clang-tidy --quiet {} -- ${CPPFLAGS}
.PHONY: lint-clang-tidy

lint-cppcheck:
	cd ${.CURDIR} && cppcheck ${CPPCHECKFLAGS} ${CPPCHECK}
.PHONY: lint-cppcheck

IWYU?=	include-what-you-use
lint-include-what-you-use:
	cd ${.CURDIR} && echo ${CPPCHECK} | xargs printf '%s\n' | \
		xargs -I{} ${IWYU} ${CPPFLAGS} {}
.PHONY: lint-include-what-you-use

lint-shellcheck:
	cd ${.CURDIR} && shellcheck ${SHELLCHECKFLAGS} ${SHLINT}
.PHONY: lint-shellcheck

test: ${PROG_knfmt} test-${PROG_test}
	${MAKE} -C ${.CURDIR}/tests "KNFMT=${.OBJDIR}/${PROG_knfmt}"
.PHONY: test

test-${PROG_test}: ${PROG_test}
	${EXEC} ./${PROG_test}

-include ${DEPS_knfmt}
-include ${DEPS_test}

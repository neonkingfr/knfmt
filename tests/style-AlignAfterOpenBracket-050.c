/*
 * AlignAfterOpenBracket: Align
 * AlignOperands: true
 * BreakBeforeBinaryOperators: All
 * ContinuationIndentWidth: 8
 * UseTab: Never
 */

int
main(void)
{
        if (0) {
                if (fffffffffffffff(aaa,
                        CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC) & FOO(42))
			return 0;
                if (fffffffffffffff(aaa,
                        CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC) & FOO(42))
			return 0;
                bool bbbbbbb = fffffffffffffff(aaa,
                        CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC) & FOO(42);
        }
}

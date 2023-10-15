/*
 * Inline assembler, cpp operands.
 */

int
main(void)
{
	__asm__ volatile (
	    HYPERCALL_LABEL
	    : HYPERCALL_OUT1 HYPERCALL_OUT2
	      HYPERCALL_OUT3
	    : HYPERCALL_IN1	HYPERCALL_IN2
	    , HYPERCALL_PTR(hcall)
	    : HYPERCALL_CLOBBER);
}

/*
 * Long expression, cast precedence bug.
 */

int
main(void)
{
	if (0) {
		if (1) {
			if (2) {
				if (i >= -(int)right->atoms.len && i <= (int)left->atoms.len) {
					return 0;
				}
			}
		}
	}
}

/*
 * Missing curly braces around single statement spanning multiple lines.
 */

int
main(void)
{
	if (0) {
		if (1) {
			if (2) {
				TOKEN_RANGE_FOREACH(ident, &dv->dv_ident, tmp)
					after = lexer_move_after(sd->sd_lx,
					    after, ident);
			}
		}
	}
}

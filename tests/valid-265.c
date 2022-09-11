/*
 * Extraneous new line in trailing cpp macro.
 */

#define error_write(er, fmt, ...) do {					\
	buffer_appendv(error_get_buffer((er)), (fmt), __VA_ARGS__);	\
	error_flush((er), 0);						\
} while (0)


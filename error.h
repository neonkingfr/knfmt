struct error	*error_alloc(int);
void		 error_free(struct error *);
struct buffer	*error_begin(struct error *);
void		 error_end(struct error *);
void		 error_reset(struct error *);
void		 error_flush(struct error *, int);

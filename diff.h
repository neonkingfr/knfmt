struct files;
struct options;

struct diffchunk {
	unsigned int	du_beg;
	unsigned int	du_end;
};

void	diff_init(void);
void	diff_shutdown(void);
int	diff_parse(struct files *, const struct options *);
int	diff_covers(const struct diffchunk *, unsigned int);
#include "util.h"

#include "config.h"

#include <sys/stat.h>

#include <ctype.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *
strnice(const char *str, size_t len)
{
	char *buf, *p;
	size_t i;

	buf = p = malloc(4 * len + 1);
	if (buf == NULL)
		err(1, NULL);
	for (i = 0; i < len; i++) {
		unsigned char c = str[i];

		if (c == '\n') {
			*buf++ = '\\';
			*buf++ = 'n';
		} else if (c == '\t') {
			*buf++ = '\\';
			*buf++ = 't';
		} else if (c == '"') {
			*buf++ = '\\';
			*buf++ = '"';
		} else if (isprint(c)) {
			*buf++ = c;
		} else {
			int n;

			n = sprintf(buf, "\\x%02x", c);
			if (n < 0)
				err(1, "%s: sprintf", __func__);
			buf += n;
		}
	}
	*buf = '\0';
	return p;
}

/*
 * Returns the width of the last line in the given string, with respect to tabs.
 */
size_t
strwidth(const char *str, size_t len, size_t pos)
{
	size_t i;

	for (i = 0; i < len; i++) {
		if (str[i] == '\n')
			pos = 0;
		else if (str[i] == '\t')
			pos += 8 - (pos % 8);
		else
			pos += 1;
	}
	return pos;
}

/*
 * Search for the given filename starting at the current working directory and
 * traverse upwards. Returns a file descriptor to the file if found, -1 otherwise.
 * The optional nlevels argument reflects the number of directories traversed
 * upwards.
 */
int
searchpath(const char *filename, int *nlevels)
{
	struct stat sb;
	dev_t dev = 0;
	ino_t ino = 0;
	int fd = -1;
	int flags = O_RDONLY | O_CLOEXEC;
	int i = 0;
	int dirfd;

	dirfd = open(".", flags | O_DIRECTORY);
	if (dirfd == -1)
		return -1;
	if (fstat(dirfd, &sb) == -1)
		goto out;
	dev = sb.st_dev;
	ino = sb.st_ino;
	for (;;) {
		fd = openat(dirfd, filename, flags);
		if (fd >= 0)
			break;

		fd = openat(dirfd, "..", flags | O_DIRECTORY);
		close(dirfd);
		dirfd = fd;
		fd = -1;
		if (dirfd == -1)
			break;
		if (fstat(dirfd, &sb) == -1)
			break;
		if (dev == sb.st_dev && ino == sb.st_ino)
			break;
		dev = sb.st_dev;
		ino = sb.st_ino;
		i++;
	}

out:
	close(dirfd);
	if (fd != -1 && nlevels != NULL)
		*nlevels = i;
	return fd;
}

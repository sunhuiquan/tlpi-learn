#include "my_read_line_buf.h"

void myreadLineBufInit(int fd, struct rlbuf *rlbuf)
{
	rlbuf->fd = fd;
	rlbuf->len = 0;
	rlbuf->next = 0;
}

ssize_t
myreadLineBuf(struct rlbuf *rlbuf, char *buffer, size_t n)
{
	ssize_t totread = 0;
	if (n <= 0 || buffer == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	for (;;)
	{
		if (rlbuf->next >= rlbuf->len)
		{
			rlbuf->len = read(rlbuf->fd, rlbuf->buf, MAX_RLBUF);
			if (rlbuf->len == -1)
				return -1;
			if (rlbuf->len == 0)
				break;

			rlbuf->next = 0;
		}

		if (totread < n - 1)
			buffer[totread++] = rlbuf->buf[rlbuf->next++];

		if (buffer[totread - 1] == '\n')
			break;
	}

	buffer[totread] = '\0';
	return totread;
}

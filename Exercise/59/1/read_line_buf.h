#ifndef READ_LINE_BUF_H
#define READ_LINE_BUF_H

#include <errno.h>
#include <unistd.h>

#define MAX_RLBUF 8192

struct rlbuf
{
	int len;
	int next;
	int fd;
	char buf[MAX_RLBUF];
};

void readLineBufInit(int fd, struct rlbuf *rlbuf);

ssize_t readLineBuf(struct rlbuf *rlbuf, char *buffer, size_t n);

#endif

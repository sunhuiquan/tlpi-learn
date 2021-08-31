#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1000

char *currTime(const char *format);

int main()
{
	int fd;

	fd = open("./file", O_RDONLY); /* Open file to be locked */
	if (fd == -1)
		errExit("open");

	if (flock(fd, LOCK_EX) == -1)
	{
		if (errno == EWOULDBLOCK)
			errExit("nonblock and already lock");
		else
			errExit("flock - LOCK_SH");
	}
	printf("PID %ld: have got exclusive lock at %s\n", (long)getpid(), currTime("%T"));

	exit(EXIT_SUCCESS); // close fd and this cause unlock flock's lock
}

char *currTime(const char *format)
{
	static char buf[BUF_SIZE]; /* Nonreentrant */
	time_t t;
	size_t s;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	if (tm == NULL)
		return NULL;

	s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);

	return (s == 0) ? NULL : buf;
}

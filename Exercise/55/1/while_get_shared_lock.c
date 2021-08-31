#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1000

char *currTime(const char *format);

int main(int argc, char *argv[])
{
	int fd;

	fd = open(argv[1], O_RDONLY); /* Open file to be locked */
	if (fd == -1)
		errExit("open");

	// if (flock(fd, LOCK_SH) == -1)
	// {
	// 	if (errno == EWOULDBLOCK)
	// 		errExit("nonblock and already lock");
	// 	else
	// 		errExit("flock - LOCK_SH");
	// }
	// printf("PID %ld: have got shared at %s\n", (long)getpid(), currTime("%T"));

	// if (flock(fd, LOCK_UN) == -1)
	// 	errExit("flock - LOCK_UN");
	// printf("PID %ld: have released shared at %s\n", (long)getpid(), currTime("%T"));

	exit(EXIT_SUCCESS);
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

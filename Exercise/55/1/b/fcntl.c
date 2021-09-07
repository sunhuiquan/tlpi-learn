#include <sys/file.h>
#include <fcntl.h>
#include <string.h>
#include "tlpi_hdr.h"

char *currTime(const char *format);

int main(int argc, char *argv[])
{
	int fd;
	struct flock fl;

	fd = open("./file", O_RDWR); /* Open file to be locked */
	if (fd == -1)
		errExit("open");

	fl.l_len = 0;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_type = F_WRLCK;

	if (fcntl(fd, F_SETLKW, &fl) == -1)
	{
		if (errno == EAGAIN || errno == EACCES)
			printf("already locked");
		else if (errno == EDEADLK)
			printf("dead lock");
		else
			errExit("fcntl");
	}
	else
		printf("PID %ld: have got write lock at %s\n", (long)getpid(), currTime("%T"));

	sleep(atoi(argv[1]));

	exit(EXIT_SUCCESS); // close fd and this cause unlock flock's lock
}

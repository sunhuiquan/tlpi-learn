// c -> a
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1000

char *currTime(const char *format);

int main(int argc, char *argv[])
{
	int fd1, fd2;

	fd1 = open("./c", O_RDONLY);
	if (fd1 == -1)
		errExit("open");

	fd2 = open("./a", O_RDONLY);
	if (fd2 == -1)
		errExit("open");

	printf("PID %ld: requesting file c at %s\n", (long)getpid(),
		   currTime("%T"));
	if (flock(fd1, LOCK_EX) == -1)
	{
		if (errno == EWOULDBLOCK)
			fatal("PID %ld: already locked file c !", (long)getpid());
		else
			errExit("File c flock (PID=%ld)", (long)getpid());
	}
	sleep(5);

	printf("PID %ld: requesting file a at %s\n", (long)getpid(),
		   currTime("%T"));
	if (flock(fd2, LOCK_EX) == -1)
	{
		if (errno == EWOULDBLOCK)
			fatal("PID %ld: already locked file a !", (long)getpid());
		else
			errExit("File a flock (PID=%ld)", (long)getpid());
	}
	sleep(5);

	if (flock(fd2, LOCK_UN) == -1)
		errExit("flock");
	printf("PID %ld: released file a at %s\n", (long)getpid(),
		   currTime("%T"));

	if (flock(fd1, LOCK_UN) == -1)
		errExit("flock");
	printf("PID %ld: released file c at %s\n", (long)getpid(),
		   currTime("%T"));

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

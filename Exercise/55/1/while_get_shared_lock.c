#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1000

void while_request_shared_lock(int secs);
char *currTime(const char *format);

int main()
{
	pid_t pid;

	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	case 0: // child
		sleep(1);
		while_request_shared_lock(2);
		break;

	default: // parent
		while_request_shared_lock(2);
		break;
	}

	// never gets here
	exit(EXIT_SUCCESS);
}

void while_request_shared_lock(int secs)
{
	int fd;

	fd = open("./file", O_RDONLY); /* Open file to be locked */
	if (fd == -1)
		errExit("open");

	for (;;)
	{
		if (flock(fd, LOCK_SH) == -1)
		{
			if (errno == EWOULDBLOCK)
				errExit("nonblock and already lock");
			else
				errExit("flock - LOCK_SH");
		}
		printf("PID %ld: have got shared at %s\n", (long)getpid(), currTime("%T"));

		sleep(secs);

		if (flock(fd, LOCK_UN) == -1)
			errExit("flock - LOCK_UN");
		printf("PID %ld: have released shared at %s\n", (long)getpid(), currTime("%T"));
	}
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

#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include "tlpi_hdr.h"

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
	struct flock fl;

	fd = open("./file", O_RDWR); /* Open file to be locked */
	if (fd == -1)
		errExit("open");

	fl.l_len = 0;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;

	for (;;)
	{
		fl.l_type = F_RDLCK;
		if (fcntl(fd, F_SETLK, &fl) == -1)
		{
			if (errno == EAGAIN || errno == EACCES)
				printf("already locked");
			else if (errno == EDEADLK)
				printf("dead lock");
			else
				errExit("fcntl");
		}
		else
			printf("PID %ld: have got read lock at %s\n", (long)getpid(), currTime("%T"));

		sleep(secs);

		fl.l_type = F_UNLCK;
		if (fcntl(fd, F_SETLK, &fl) == -1)
			errExit("fcntl");
		else
			printf("PID %ld: have released read lock at %s\n", (long)getpid(), currTime("%T"));
	}
}

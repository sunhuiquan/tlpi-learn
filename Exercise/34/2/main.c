#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <tlpi_hdr.h>

int main()
{
	pid_t pid, pgid;

	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	case 0:
#ifndef EXEC
		sleep(30);
#else
		execl("./sleep", "./sleep", NULL);
#endif
		_exit(EXIT_SUCCESS);

	default:
		break;
	}

	sleep(3); // ensure child has exec()

	if ((pgid = getpgid(pid)) == -1)
		errExit("getpgid");
	printf("Before: %ld\n", (long)pgid);

	if (setpgid(pid, pid) == -1)
	{
		if (errno == EACCES)
			printf("Can't modify a child process's PGID after exec()\n");
		else
			errExit("setpgid");
	}

	if ((pgid = getpgid(pid)) == -1)
		errExit("getpgid");
	printf("After: %ld\n", (long)pgid);

	return 0;
}
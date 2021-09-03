#include <unistd.h>
#include <stdlib.h>
#include <tlpi_hdr.h>

int main()
{
	pid_t sid, pid;

	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	case 0:
		sleep(3);

		if ((sid = getsid(pid)) == -1)
			errExit("getsid");
		printf("\nBefore: %ld\n", (long)sid);

#ifdef FIRST_IN_PG
		if (setpgid(0, 0) == -1)
			errExit("setgid");
		printf("Child process is the first one in process group.\n");
#else
		printf("Child process is not the first one in process group.\n");
#endif

		if (setsid() == -1)
		{
			if (errno == EPERM)
				printf("Can't setsid if process is the first one in process group.\n");
			else
				errExit("setsid");
		}

		if ((sid = getsid(pid)) == -1)
			errExit("getsid");
		printf("After: %ld\n", (long)sid);
		_exit(EXIT_SUCCESS);
		break;

	default:
		break;
	}
	return 0;
}
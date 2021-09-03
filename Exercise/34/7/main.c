#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <tlpi_hdr.h>

void handler(int sig)
{
	if (sig == SIGTTIN)
		printf("get sigttin\n"); // unsafe can be race condition here!!
}

// 孤儿进程组会忽略SIGTTIN、SIGTTOU、SIGTSTP，除非建立了handler
// 这些信号造成进程停止，因为一旦孤儿进程组被停止后，则无法正常唤醒
int main()
{
	pid_t pid;
	struct sigaction act;

	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	case 0:
		sleep(2); // 确保成为孤儿进程组

#ifdef WITH_HANDLER
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		act.sa_handler = handler;
		if (sigaction(SIGTTIN, &act, NULL) == -1)
			errExit("sigaction");
#endif

		while (1)
		{
			pause();
			printf("get a signal\n");
		}
		_exit(EXIT_SUCCESS);
		break;

	default:
		break;
	}
	exit(EXIT_SUCCESS);
}
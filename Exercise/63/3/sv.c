#include <sys/select.h>
#include <tlpi_hdr.h>

#define MAXLINE 1024

int main()
{
	int pid, maxfd, readn;
	int pfd[2];
	fd_set rfdset;
	char buf[MAXLINE];

	if (pipe(pfd) == -1)
		errExit("pipe");
	if ((pid = fork()) == -1)
		errExit("fork");

	if (pid == 0)
	{
		close(pfd[0]);
		// to do: copy data from system V message queue to pipe

		// if(write) // -1是因为主进程已终止，所以应该这里应该终止
		_exit(EXIT_SUCCESS);
	}

	close(pfd[1]);
	FD_ZERO(&rfdset);
	FD_SET(STDIN_FILENO, &rfdset);
	FD_SET(pfd[0], &rfdset);
	maxfd = max(pfd[0], STDIN_FILENO) + 1;

	while (true)
	{
		if (select(maxfd, &rfdset, NULL, NULL, NULL) == -1)
			errExit("select");

		if (FD_ISSET(STDIN_FILENO, &rfdset))
		{
			if ((readn = read(STDIN_FILENO, buf, MAXLINE)) < 0)
				errExit("read");
			if (readn == 0)
				break; // 终端输入EOF(ctrl + D)

			if (write(STDOUT_FILENO, buf, readn) != readn)
				errExit("write");
		}

		if (FD_ISSET(pfd[0], &rfdset))
		{
			if ((readn = read(STDIN_FILENO, buf, MAXLINE)) < 0)
				errExit("read");
			if (readn == 0)
				break; // 子进程终止(这不是期待的结果)

			if (write(STDOUT_FILENO, buf, readn) != readn)
				errExit("write");
		}
	}

	exit(EXIT_SUCCESS);
}
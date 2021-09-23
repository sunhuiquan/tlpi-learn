#include <sys/select.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <tlpi_hdr.h>

#define MAXLINE 1024
#define MAX_MTEXT 1024

struct mbuf
{
	long mtype;
	char mtext[MAX_MTEXT];
};

int main()
{
	int pid, maxfd, readn, msglen;
	int pfd[2];
	fd_set rfdset;
	char buf[MAXLINE];

	int msqid; // 这个不是fd注意
	struct mbuf msg;

	if (pipe(pfd) == -1)
		errExit("pipe");
	if ((pid = fork()) == -1)
		errExit("fork");

	if (pid == 0)
	{
		close(pfd[0]);

		// copy data from system V message queue to pipe
		if ((msqid = msgget(IPC_PRIVATE, 0666)) == -1)
			errExit("msgget");
		printf("%d\n", msqid); // 为了让另一个能获取到对应的消息队列(写入一个文件来共享其实更好)

		while (true)
		{
			if ((msglen = msgrcv(msqid, &msg, MAX_MTEXT, 0, 0)) == -1) // msglen是读入mtext字段的实际长度
				errExit("msgrcv");

			if (write(pfd[1], msg.mtext, msglen) != msglen)
				errExit("write");
		}
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
				break; // 子进程终止(这不是期待的结果，只有子进程错误终止才会这样)

			if (write(STDOUT_FILENO, buf, readn) != readn)
				errExit("write");
		}
	}

	exit(EXIT_SUCCESS);
}
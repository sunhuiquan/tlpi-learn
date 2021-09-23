#include <sys/select.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/poll.h>
#include <tlpi_hdr.h>

#define MAXLINE 1024
#define MAX_MTEXT 1024

struct mbuf
{
	long mtype;
	char mtext[MAX_MTEXT];
};

void sigalarm_handler(int sig)
{
	return; // do nothing, just interupt a system call
}

int is_pipe_closed(int fd)
{
	struct pollfd pfd = {
		.fd = fd,
		.events = POLLOUT,
	};

	if (poll(&pfd, 1, 1) < 0)
		return 0;
	return (pfd.revents & POLLERR) ? 1 : 0;
}

int main()
{
	int pid, maxfd, readn, msglen;
	int pfd[2];
	fd_set rfdset;
	char buf[MAXLINE];

	int msqid; // 这个不是fd注意
	struct mbuf msg;
	struct sigaction act;

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
		printf("msqid: %d\n", msqid); // 为了让另一个能获取到对应的消息队列(写入一个文件来共享其实更好)

		sigemptyset(&act.sa_mask);
		act.sa_handler = sigalarm_handler;
		act.sa_flags = 0;
		if (sigaction(SIGALRM, &act, NULL) == -1)
			errExit("sigaction");

		while (true)
		{
			alarm(3); // 这个技术主要是为了方式阻塞在msgrcv导致无法发现主进程已关闭
			// 但是由于system V不可以被IO多路复用检测，所以可以通过父进程发一个信号也行，不过这次我用这个定时的方式
			if ((msglen = msgrcv(msqid, &msg, MAX_MTEXT, 0, 0)) == -1 && errno != EINTR) // msglen是读入mtext字段的实际长度
				errExit("msgrcv");

			alarm(0); // 避免打断后面的调用

			// to do 检测pipe对端是否关闭
			if (is_pipe_closed(pfd[1]))
			{
				if (msgctl(msqid, IPC_RMID, NULL) == -1)
					errExit("msgctl");
				_exit(EXIT_SUCCESS);
			}

			if (msglen != -1 && write(pfd[1], msg.mtext, msglen) != msglen)
				errExit("write");
		}
		_exit(EXIT_SUCCESS); // never end here
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
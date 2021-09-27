#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <utmp.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <tlpi_hdr.h>
#include <time.h>
#include <signal.h>
#include "../../../tlpi-dist/pty/pty_fork.h"	  /* Declaration of ptyFork() */
#include "../../../tlpi-dist/tty/tty_functions.h" /* Declaration of ttySetRaw() */

#define SSH_SERVICE 22
#define MAXLINE 1024
#define BUF_SIZE 256
#define MAX_SNAME 1000
#define MAXTIMELEN 128

struct termios ttyOrig;

int do_ssh(int connfd);
int readline(int fd, char *buf, int sz);

void sigchld_handler(int sig)
{
	int saved = errno;
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
	errno = saved;
}

// 注意这个程序以root身份运行，因为设置22端口和执行login程序都需要root权限
int main()
{
	int lfd, connfd;
	struct sockaddr_in addr;
	struct sigaction act;

	if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SSH_SERVICE);
	if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) == -1)
		errExit("inet_pton");

	if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("bind");
	if (listen(lfd, 5) == -1)
		errExit("listen");

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	act.sa_handler = sigchld_handler;
	if (sigaction(SIGCHLD, &act, NULL) == -1)
		errExit("sigaction");

	for (;;)
	{
		if ((connfd = accept(lfd, NULL, NULL)) == -1)
			errExit("accept");

		switch (fork())
		{
		case -1:
			errExit("fork");
			break;

		case 0:
			close(lfd);
			if (do_ssh(connfd) == -1)
				errExit("do_ssh");
			_exit(EXIT_SUCCESS);
			break;

		default:
			close(connfd);
			break;
		}
	}

	return 0;
}

int do_ssh(int connfd)
{
	char slaveName[MAX_SNAME];
	int masterFd;
	struct winsize ws;
	fd_set inFds;
	char buf[BUF_SIZE];
	ssize_t numRead;
	pid_t childPid;
	char user[MAXLINE];

	// 读用户名
	if (readline(connfd, user, MAXLINE) <= 0) // 用readline因为我们用了\n做分界
		return -1;
	user[strlen(user) - 1] = '\0'; // 删除\n变\0

	// -----------------------------------------------------

	/* Retrieve the attributes of terminal on which we are started */

	if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
		// 这里是主设备绑定的服务器的实际终端，后面根本用不到这些
		errExit("tcgetattr");
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
		errExit("ioctl-TIOCGWINSZ");

	/* Create a child process, with parent and child connected via a
       pty pair. The child is connected to the pty slave and its terminal
       attributes are set to be the same as those retrieved above. */

	childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws);
	if (childPid == -1)
		errExit("ptyFork");

	if (childPid == 0)
	{
		printf("%ld\n", (long)getpid());
		// child
		execlp("login", "login", user, (char *)NULL);
		// 变参NULL要手动转函数原型的类型，只是因为编译器的做法问题，比如定参就不用这样做
		errExit("execlp"); /* If we get here, something went wrong */
	}

	// parent
	for (;;)
	{
		FD_ZERO(&inFds);
		FD_SET(connfd, &inFds);
		FD_SET(masterFd, &inFds);

		if (select(max(connfd, masterFd) + 1, &inFds, NULL, NULL, NULL) == -1)
		{
			if (errno = EINTR)
				continue;
			else
				errExit("select");
		}

		if (FD_ISSET(connfd, &inFds))
		{ /* stdin --> pty */
			numRead = read(connfd, buf, BUF_SIZE);
			if (numRead <= 0) // 正常退出^D也是通过这个途径
				exit(EXIT_SUCCESS);

			if (write(masterFd, buf, numRead) != numRead)
				fatal("partial/failed write (masterFd)");
		}

		if (FD_ISSET(masterFd, &inFds))
		{ /* pty --> stdout+file */
			numRead = read(masterFd, buf, BUF_SIZE);
			if (numRead <= 0) // 正常退出也是通过这个途径
				exit(EXIT_SUCCESS);

			if (write(connfd, buf, numRead) != numRead)
				fatal("partial/failed write (STDOUT_FILENO)");
		}
	}

	return 0;
}

int readline(int fd, char *buf, int sz)
{
	char buffer[MAXLINE], ch; // 静态变量初始值就是0，只有局部变量才要初始化
	int index, readn;

	index = 0;
	while (true)
	{
		if ((readn = read(fd, &ch, 1)) <= 0)
			return readn;

		buffer[index++] = ch;
		if (ch == '\n')
		{
			buffer[index] = '\0';
			strncpy(buf, buffer, MAXLINE); // 这个是把'\0'也复制过去
			return index;
		}
	}
}
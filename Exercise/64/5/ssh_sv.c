#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <tlpi_hdr.h>
#include <time.h>
#include <signal.h>
#include "../../../tlpi-dist/pty/pty_fork.h"	  /* Declaration of ptyFork() */
#include "../../../tlpi-dist/tty/tty_functions.h" /* Declaration of ttySetRaw() */

#define SSH_SERVICE 9999
#define MAXLINE 1024
#define BUF_SIZE 256
#define MAX_SNAME 1000
#define MAXTIMELEN 128

struct termios ttyOrig;

struct termios ttyOrig;

int do_ssh(int connfd);
int readline(int fd, char *buf, int sz);

static void /* Reset terminal mode on program exit */
ttyReset(void)
{
	if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1)
		errExit("tcsetattr");
}

// 注意这个程序以root身份运行，因为设置22端口和执行login程序都需要root权限
int main()
{
	int lfd, connfd;
	struct sockaddr_in addr;

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

	char user[MAXLINE];

	// 读用户名
	if (readline(connfd, user, MAXLINE) <= 0) // 用readline因为我们用了\n做分界
		return -1;
	user[strlen(user) - 1] = '\0'; // 删除\n变\0

	// -----------------------------------------------------

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
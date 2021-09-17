#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <tlpi_hdr.h>
#include "shell_h.h"

void serve_func(int connfd);
void _exit_write(char *msg);

int main()
{
	int lfd, connfd;
	struct sockaddr_in addr;

	if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUMBER);
	if (inet_pton(AF_INET, HOST_ADDRESS, &addr.sin_addr) == -1)
		errExit("inet_pton");

	if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("bind");
	// pr_sockname_inet4(lfd, &addr); // 打印验证下是不是绑定没问题
	if (listen(lfd, 10) == -1)
		errExit("listen");

	while (true)
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
			serve_func(connfd);
			_exit(EXIT_SUCCESS);
			break;

		default:
			close(connfd);
			break;
		}
	}

	return 0;
}

void serve_func(int connfd)
{
	int readn;
	char buf[MAXLINE], command[MAXLINE], buf2[MAXLINE + 100];
	if (dup2(connfd, STDOUT_FILENO) == -1)
		_exit_write("dup2");
	if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1)
		_exit_write("dup2");

	command[0] = '\0';
	while ((readn = read(connfd, buf, MAXLINE)) > 0)
		strncpy(command, buf, MAXLINE);
	if (readn < 0)
		_exit_write("read");

	snprintf(buf2, MAXLINE + 100, "Excute \"%s\" command:\n", command);
	if (write(STDOUT_FILENO, buf2, strlen(buf2)) != strlen(buf2))
		_exit_write("write");
	system(command);

	return;
}

void _exit_write(char *msg)
{
	write(STDOUT_FILENO, msg, strlen(msg));
	_exit(EXIT_FAILURE);
}
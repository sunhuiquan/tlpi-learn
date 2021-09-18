#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <tlpi_hdr.h>

#define PORT_NUMBER 9990
#define MAXLINE 1024

void serve_func(int connfd, struct sockaddr_in *addr);
void _exit_write(char *msg);

int main()
{
	int lfd, connfd;
	struct sockaddr_in addr, cli_addr;
	socklen_t cli_len;

	if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUMBER);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("bind");

	if (listen(lfd, 10) == -1)
		errExit("listen");

	while (true)
	{
		cli_len = sizeof(cli_addr);
		if ((connfd = accept(lfd, (struct sockaddr *)&cli_addr, &cli_len)) == -1)
			errExit("accept");

		switch (fork())
		{
		case -1:
			errExit("fork");
			break;

		case 0:
			close(lfd);
			serve_func(connfd, &cli_addr);
			_exit(EXIT_SUCCESS);
			break;

		default:
			close(connfd);
			break;
		}
	}

	return 0;
}

void serve_func(int connfd, struct sockaddr_in *addr)
{
	int prior_connfd;

	if ((prior_connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	// 除了port，其他的不需要变
	if (read(connfd, &addr->sin_port, sizeof(addr->sin_port)) != sizeof(addr->sin_port))
		_exit_write("read");

	if (connect(prior_connfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("connect");

	// connect返回后就建立了两条连接了
	sleep(5); // 为了体现出如果同时到了两种数据我们会先处理优先数据
}

void _exit_write(char *msg)
{
	write(STDOUT_FILENO, msg, strlen(msg));
	_exit(EXIT_FAILURE);
}

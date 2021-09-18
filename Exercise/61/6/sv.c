#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h> // struct time_val
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
	int prior_connfd, readn, close_connfd, close_prior_connfd;
	fd_set rfdset;
	char buf[MAXLINE];

	if ((prior_connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		_exit_write("socket");

	// 除了port，其他的不需要变
	if (read(connfd, &addr->sin_port, sizeof(addr->sin_port)) != sizeof(addr->sin_port))
		_exit_write("read");

	if (connect(prior_connfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		_exit_write("connect");

	// connect返回后就建立了两条连接了
	sleep(5); // 为了体现出如果同时到了两种数据我们会先处理优先数据

	close_connfd = 0;
	close_prior_connfd = 0;
	FD_ZERO(&rfdset);
	while (true)
	{
		if (close_connfd && close_prior_connfd)
			break;

		if (!close_connfd)
			FD_SET(connfd, &rfdset);
		if (!close_prior_connfd)
			FD_SET(prior_connfd, &rfdset);
		if (select(max(connfd, prior_connfd) + 1, &rfdset, NULL, NULL, NULL) == -1)
			_exit_write("select");

		if (FD_ISSET(prior_connfd, &rfdset)) // 同时到达的情况优先数据先读
		{
			// 不用循环读，因为如果没读完，下一次循环select不会阻塞立马可以接着读
			// 如果里面也循环那么一样要非阻塞,避免读过多导致阻塞
			readn = read(prior_connfd, buf, MAXLINE);
			if (readn < 0)
				_exit_write("read");
			else if (readn == 0)
			{
				close_prior_connfd = 1;
				FD_CLR(prior_connfd, &rfdset);
			}
		}

		if (FD_ISSET(connfd, &rfdset))
		{
			readn = read(connfd, buf, MAXLINE);
			if (readn < 0)
				_exit_write("read");
			else if (readn == 0)
			{
				close_connfd = 1;
				FD_CLR(connfd, &rfdset);
			}
		}
	}
	return;
}

void _exit_write(char *msg)
{
	write(STDOUT_FILENO, msg, strlen(msg));
	_exit(EXIT_FAILURE);
}

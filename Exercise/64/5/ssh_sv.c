#include <inet_sockets.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <tlpi_hdr.h>

#define SSH_SERVICE 22
#define MAXLINE 1024

int do_ssh(int connfd);

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
	// 读用户名
}

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <tlpi_hdr.h>

#define PORT_NUMBER 9990
#define MAXLINE 1024

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
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("bind");

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
	// to do
}

void _exit_write(char *msg)
{
	write(STDOUT_FILENO, msg, strlen(msg));
	_exit(EXIT_FAILURE);
}

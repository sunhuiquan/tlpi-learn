#include <inet_sockets.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <tlpi_hdr.h>

#define SSH_SERVICE 22
#define MAXLINE 1024

int main()
{
	int cfd;
	struct sockaddr_in addr;
	int readn, closed;
	char buf[MAXLINE];
	fd_set rfdset;

	if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SSH_SERVICE);
	if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) == -1)
		errExit("inet_pton");

	if (connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("connect");

	FD_ZERO(&rfdset);

	closed = 0;
	for (;;)
	{
		if (!closed)
			FD_SET(STDIN_FILENO, &rfdset);
		FD_SET(cfd, &rfdset);

		if (select(cfd + 1, &rfdset, NULL, NULL, NULL) == -1)
			errExit("select");

		if (FD_ISSET(STDIN_FILENO, &rfdset))
		{
			readn = read(STDIN_FILENO, buf, MAXLINE);
			if (readn < 0)
				errExit("read");
			else if (readn == 0)
			{
				closed = 1;
				FD_CLR(STDIN_FILENO, &rfdset);
				if (shutdown(cfd, SHUT_WR) == -1)
					errExit("shutdown");
			}
		}

		if (FD_ISSET(cfd, &rfdset))
		{
		}
	}

	return 0;
}
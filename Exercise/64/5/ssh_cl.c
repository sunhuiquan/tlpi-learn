#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <tlpi_hdr.h>

#define SSH_SERVICE 22
#define MAXLINE 1024

int main(int argc, char *argv[])
{
	int cfd;
	struct sockaddr_in addr;
	int readn, closed;
	char buf[MAXLINE];
	fd_set rfdset;
	char user[MAXLINE], host[MAXLINE];
	char *pc;

	if (argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
	{
		printf("%s usage: <user@host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((pc = strchr(argv[1], '@')) == NULL)
	{
		printf("%s usage: <user@host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	*pc = '\0';
	strncpy(user, argv[1], MAXLINE);
	strncpy(host, pc + 1, MAXLINE);

	// for testing
	printf("user: %s; host: %s\n", user, host);

	if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SSH_SERVICE);
	if (inet_pton(AF_INET, host, &addr.sin_addr) == -1)
		errExit("inet_pton");

	if (connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("connect");

	FD_ZERO(&rfdset);

	// login的用户
	int index = strlen(user);
	user[index] = '\n';		// 下一行不能用strlen()，因为这一步会删掉\0导致错误
	user[index + 1] = '\0'; // \n作为分隔字节流用
	if (write(cfd, user, strlen(user)) != strlen(user))
		errExit("write");

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
				if (shutdown(cfd, SHUT_WR) == -1) // 对端收到FIN，且读connfd会得到EOF
					errExit("shutdown");
			}

			if (write(cfd, buf, readn) != readn)
				errExit("write");
		}

		if (FD_ISSET(cfd, &rfdset))
		{
			readn = read(cfd, buf, MAXLINE);
			if (readn < 0)
				errExit("read");
			else if (readn == 0) // 对端关闭(由于之前shutdown并把剩余数据读完得到EOF，导致的关闭)
			{
				printf("lagout\n");
				break; // 正常结束客户端
			}

			if (write(STDOUT_FILENO, buf, readn) != readn)
				errExit("write");
		}
	}

	return 0;
}
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <tlpi_hdr.h>
#include <tlpi_hdr.h>
#include "shell_h.h"

int main(int argc, char *argv[])
{
	int sfd, readn;
	struct sockaddr_in addr;
	char buf[MAXLINE];

	if (argc != 2)
	{
		printf("%s usage: <shell-command> \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUMBER);
	if (inet_pton(AF_INET, HOST_ADDRESS, &addr.sin_addr) == -1)
		errExit("inet_pton");
	if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("connect");

	if (write(sfd, argv[1], strlen(argv[1])) != strlen(argv[1]))
		errExit("write");
	if (shutdown(sfd, SHUT_WR) == -1)
		errExit("shutdown");

	while ((readn = read(sfd, buf, MAXLINE)) > 0)
		if (write(STDOUT_FILENO, buf, readn) != readn)
			errExit("write");
	if (readn < 0)
		errExit("read");

	return 0;
}
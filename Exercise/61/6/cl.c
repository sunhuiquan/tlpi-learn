#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <tlpi_hdr.h>

#define PORT_NUMBER 9990
#define MAXLINE 1024

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
	if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) == -1)
		errExit("inet_pton");
	if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("connect");

	// to do

	return 0;
}
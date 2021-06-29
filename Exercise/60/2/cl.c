#include <signal.h>
#include <tlpi_hdr.h>
#include "../../../tlpi-dist/sockets/inet_sockets.h"

#define BUF_SIZE 100

int main(int argc, char *argv[])
{
	int sfd;
	char buf[BUF_SIZE];
	ssize_t n;

	if ((sfd = inetConnect("localhost", "echo", SOCK_STREAM)) == -1)
		err_exit("inetConnect");

	while ((n = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
	{
		if (write(sfd, buf, n) != n)
			errExit("write");

		if ((n = read(sfd, buf, BUF_SIZE)) >= 0)
			printf("%.*s", (int)n, buf);
		if (n == -1)
			errExit("read");
	}
	if (n == -1)
		errExit("read");

	return 0;
}

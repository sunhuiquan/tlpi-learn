#include <signal.h>
#include <tlpi_hdr.h>
#include <sys/wait.h>
#include "../../../tlpi-dist/sockets/inet_sockets.h"

#define BUF_SIZE 100

int main(int argc, char *argv[])
{
	int sfd;
	char buf[BUF_SIZE];

	for (int i = 0; i < 100; ++i)
	{
		switch (fork())
		{
		case -1:
			errExit("fork");

		case 0:
			if ((sfd = inetConnect("localhost", "echo", SOCK_STREAM)) == -1)
				err_exit("inetConnect");
			sprintf(buf, "%d", i);
			if (write(sfd, buf, strlen(buf)) != strlen(buf))
				err_exit("write");
			if (read(sfd, buf, BUF_SIZE) <= 0)
				err_exit("read");
			printf("%s\n", buf);
			_exit(EXIT_SUCCESS);

		default:
			break;
		}
	}

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	return 0;
}

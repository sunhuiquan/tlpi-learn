#include <sys/types.h>
#include <sys/socket.h>
#include <tlpi_hdr.h>

#define MAXBUF 1024

int sock_pipe(int spfd[2]);

int main()
{
	char buf1[MAXBUF], buf2[MAXBUF];
	int res;
	int spfd[2];

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, spfd) == -1)
		errExit("socketpair");

	switch (fork())
	{
	case -1:
		errExit("fork");
		break;

	case 0:
		close(spfd[1]);
		if (shutdown(spfd[0], SHUT_WR) == -1)
			errExit("shutdown");
		while ((res = read(spfd[0], buf2, MAXBUF)) > 0)
			if (write(STDOUT_FILENO, buf2, res) != res)
				errExit("write");
		if (res < 0)
			errExit("read");
		_exit(EXIT_SUCCESS);
		break;

	default:
		break;
	}
	close(spfd[0]);
	if (shutdown(spfd[1], SHUT_RD) == -1)
		errExit("shutdown");
	while ((res = read(STDIN_FILENO, buf1, MAXBUF)) > 0)
		if (write(spfd[1], buf1, res) != res)
			errExit("write");
	if (res < 0)
		errExit("read");

	return 0;
}

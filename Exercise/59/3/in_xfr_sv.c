#include "in_xfr.h"
#define BACKLOG 5

int main(int argc, char *argv[])
{
	int sfd, cfd;
	ssize_t numRead;
	char buf[BUF_SIZE];
	socklen_t addrlen;

	if ((sfd = inetListen(PORT_NUMBER, 5, &addrlen)) == -1)
		errExit("inetListen");

	for (;;)
	{
		cfd = accept(sfd, NULL, NULL);
		if (cfd == -1)
			errExit("accept");

		while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				fatal("partial/failed write");

		if (numRead == -1)
			errExit("read");

		if (close(cfd) == -1)
			errMsg("close");
	}
}

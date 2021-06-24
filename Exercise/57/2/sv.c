#include "myund.h"
#define BACKLOG 5

int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	int sfd, cfd;
	ssize_t numRead;
	char buf[BUF_SIZE];

	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd == -1)
		errExit("socket");

	/* Construct server socket address, bind socket to it,
       and make this a listening socket */

	/* For an explanation of the following check, see the errata notes for
       pages 1168 and 1172 at http://www.man7.org/tlpi/errata/. */

	if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1)
		fatal("Server socket path too long: %s", SV_SOCK_PATH);

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(&addr.sun_path[1], SV_SOCK_PATH, sizeof(addr.sun_path) - 2);

	if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
		errExit("bind");

	if (listen(sfd, BACKLOG) == -1)
		errExit("listen");

	for (;;)
	{ /* Handle client connections iteratively */

		/* Accept a connection. The connection is returned on a new
           socket, 'cfd'; the listening socket ('sfd') remains open
           and can be used to accept further connections. */

		cfd = accept(sfd, NULL, NULL);
		if (cfd == -1)
			errExit("accept");

		/* Transfer data from connected socket to stdout until EOF */

		while ((numRead = read(cfd, buf, BUF_SIZE)) > 0)
			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				fatal("partial/failed write");

		if (numRead == -1)
			errExit("read");

		if (close(cfd) == -1)
			errMsg("close");
	}
}

#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <tlpi_hdr.h>
#include "und.h"

int main()
{
	int sfd, cfd;
	struct sockaddr_un unaddr;

	if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	memset(&unaddr, 0, sizeof(struct sockaddr_un));
	unaddr.sun_family = AF_UNIX;
	strncpy(unaddr.sun_path, SOCKET_PATH, sizeof(unaddr.sun_path) - 1);

	if (unlink(SOCKET_PATH) == -1 && errno != ENOENT)
		errExit("unlink");
	if (bind(sfd, (struct sockaddr *)&unaddr, sizeof(struct sockaddr_un)) == -1)
		errExit("bind");

	if (listen(sfd, 5) == -1)
		errExit("listen");

	for (;;)
	{
		if ((cfd = accept(sfd, NULL, NULL)) == -1)
			errExit("accept");
		printf("sleep\n");
		sleep(5);
	}

	return 0;
}
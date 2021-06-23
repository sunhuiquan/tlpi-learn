#include <sys/un.h>
#include <sys/socket.h>
#include <tlpi_hdr.h>
#include "und.h"

int main()
{
	int cfd;
	struct sockaddr_un unaddr;

	memset(&unaddr, 0, sizeof(struct sockaddr_un));
	unaddr.sun_family = AF_UNIX;
	strncpy(unaddr.sun_path, SOCKET_PATH, sizeof(unaddr.sun_path) - 1);

	for (int i = 0;; ++i)
	{
		if ((cfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
			errExit("socket");

		printf("connet %d\n", i);
		if (connect(cfd, (struct sockaddr *)&unaddr, sizeof(struct sockaddr_un)) == -1)
			errExit("connect");
	}

	return 0;
}
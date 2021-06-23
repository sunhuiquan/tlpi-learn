#include "u.h"
#include <tlpi_hdr.h>

int main()
{
	int cfd;
	struct sockaddr_un svaddr;

	if ((cfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
		errExit("socket");

	memset(&svaddr, 0, sizeof(struct sockaddr_un));
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SOCKET_PATH, sizeof(svaddr.sun_path) - 1);

	for (int i = 0;; ++i)
	{
		printf("send %d\n", i);
		if (sendto(cfd, "a", 1, 0, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) != 1)
			errExit("sendto");
	}

	return 0;
}
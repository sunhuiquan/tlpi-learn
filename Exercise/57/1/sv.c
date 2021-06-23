#include "u.h"
#include <tlpi_hdr.h>

int main()
{
	int sfd;
	struct sockaddr_un addr;
	char msg[MSG_SZ];

	if ((sfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
		errExit("socket");

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

	if (unlink(SOCKET_PATH) == -1 && errno != ENOENT)
		errExit("unlink");

	if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
		errExit("bind");

	for (int i = 0;; ++i)
	{
		sleep(1);
		if (recv(sfd, msg, MSG_SZ, 0) == -1)
			errExit("recv");
		printf("%s %d\n", msg, i);
	}

	return 0;
}

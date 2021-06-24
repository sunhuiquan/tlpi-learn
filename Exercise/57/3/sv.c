#include "myund.h"

int main()
{
	int sfd, cofd;
	struct sockaddr_un addr;
	struct request req;
	struct response res;
	int seq = 0;

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(&addr.sun_path[1], SOCKET_PATH, sizeof(addr.sun_path) - 2);

	if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		errExit("socket");
	if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
		errExit("bind");
	if (listen(sfd, 5) == -1)
		errExit("listen");

	for (;;)
	{
		if ((cofd = accept(sfd, NULL, NULL)) == -1)
			errExit("accept");

		if (recv(cofd, &req, sizeof(struct request), 0) == -1)
			errExit("recv");

		res.seqNum = seq;
		if (send(cofd, &res, sizeof(struct response), 0) != sizeof(struct response))
			errExit("send");

		seq += req.seqLen;
	}
	return 0;
}

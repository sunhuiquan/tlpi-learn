#include "myund.h"

int main(int argc, char *argv[])
{
	int cfd;
	struct sockaddr_un addr;
	struct request req;
	struct response res;

	if (argc != 2 || *argv[1] == '-')
	{
		printf("%s usage: seq-num\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	req.seqLen = atoi(argv[1]);

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(&addr.sun_path[1], SOCKET_PATH, sizeof(addr.sun_path) - 2);

	if ((cfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		errExit("socket");
	if (connect(cfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
		errExit("connect");
	if (send(cfd, &req, sizeof(struct request), 0) != sizeof(struct request))
		errExit("send");
	if (recv(cfd, &res, sizeof(struct response), 0) != sizeof(struct response))
		errExit("recv");

	printf("%d\n", res.seqNum);

	return 0;
}
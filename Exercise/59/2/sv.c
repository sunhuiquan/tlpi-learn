#include <netdb.h>
#include "ind.h"

#define BACKLOG 50

int main(int argc, char *argv[])
{
	uint32_t seqNum;
	char reqLenStr[INT_LEN]; /* Length of requested sequence */
	char seqNumStr[INT_LEN]; /* Start of granted sequence */
	struct sockaddr_storage claddr;
	int lfd, cfd, reqLen;
	socklen_t addrlen;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
	char addrStr[ADDRSTRLEN];

	if (argc > 1 && strcmp(argv[1], "--help") == 0)
		usageErr("%s [init-seq-num]\n", argv[0]);

	seqNum = (argc > 1) ? getInt(argv[1], 0, "init-seq-num") : 0;

	/* Ignore the SIGPIPE signal, so that we find out about broken connection
       errors via a failure from write(). */

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		errExit("signal");

	if ((lfd = inetListen(PORT_NUM, BACKLOG, &addrlen)) == -1)
		errExit("inetListen");

	for (;;)
	{ /* Handle clients iteratively */

		/* Accept a client connection, obtaining client's address */

		addrlen = sizeof(struct sockaddr_storage);
		cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
		if (cfd == -1)
		{
			errMsg("accept");
			continue;
		}

		inetAddressStr((struct sockaddr *)&claddr, addrlen, addrStr, ADDRSTRLEN);
		printf("Connection from %s\n", addrStr);

		/* Read client request, send sequence number back */

		if (readLine(cfd, reqLenStr, INT_LEN) <= 0)
		{
			close(cfd);
			continue; /* Failed read; skip request */
		}

		reqLen = atoi(reqLenStr);
		if (reqLen <= 0)
		{ /* Watch for misbehaving clients */
			close(cfd);
			continue; /* Bad request; skip it */
		}

		snprintf(seqNumStr, INT_LEN, "%d\n", seqNum);
		if (write(cfd, seqNumStr, strlen(seqNumStr)) != strlen(seqNumStr))
			fprintf(stderr, "Error on write");

		seqNum += reqLen; /* Update sequence number */

		if (close(cfd) == -1) /* Close connection */
			errMsg("close");
	}
}

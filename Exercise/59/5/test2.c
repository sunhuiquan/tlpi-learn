#include <tlpi_hdr.h>
#include "myinet.h"

int main()
{
	int c;
	struct addrinfo hints;
	struct addrinfo *res;

	if ((c = inetConnect("localhost", "50001", SOCK_DGRAM)) == -1)
		errExit("inetConnect");

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_canonname = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if (getaddrinfo("localhost", "50001", &hints, &res) != 0)
		errExit("getaddrinfo");

	if (sendto(c, "aaa", 3, 0, res->ai_addr, res->ai_addrlen) != 3)
		errExit("sendto");

	return 0;
}

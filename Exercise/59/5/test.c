#include <tlpi_hdr.h>
#include "myinet.h"

int main()
{
	struct addrinfo hints;
	struct addrinfo *res;
	int a, b;
	socklen_t len_b;
	int n;
	char buf[10];

	if ((a = inetBind("50001", SOCK_DGRAM, NULL)) == -1)
		errExit("inetBind");
	if ((b = inetBind("50002", SOCK_DGRAM, &len_b)) == -1)
		errExit("inetBind");

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_canonname = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if (getaddrinfo("localhost", "50002", &hints, &res) != 0)
		errExit("getaddrinfo");
	if (connect(a, res->ai_addr, res->ai_addrlen) == -1)
		errExit("connect");
	freeaddrinfo(res);

	n = recv(a, buf, 10, 0);
	if (n == -1)
		errExit("recv");
	buf[n] = '\0';
	printf("%s\n", buf);

	return 0;
}
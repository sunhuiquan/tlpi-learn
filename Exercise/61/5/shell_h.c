#include "shell_h.h"

int pr_sockname_inet4(int sfd, struct sockaddr_in *addr)
{
	char addrstr[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &addr->sin_addr, addrstr, sizeof(addrstr)) == NULL)
		return -1;
	printf("%s : %u\n", addrstr, ntohs(addr->sin_port));
	return 0;
}

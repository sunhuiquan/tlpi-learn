#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tlpi_hdr.h>

int main()
{
	int sfd;
	struct sockaddr_in addr;
	socklen_t len;
	char addrstr[INET_ADDRSTRLEN];

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	printf("Before listen, kernel hasn't aportion a address for it: ");
	len = sizeof(addr);
	if (getsockname(sfd, (struct sockaddr *)&addr, &len) == -1)
		errExit("getsockname");
	if (inet_ntop(AF_INET, &addr.sin_addr, addrstr, sizeof(addrstr)) == NULL)
		errExit("inet_ntop");
	printf("%s : %u\n", addrstr, ntohs(addr.sin_port));

	if (listen(sfd, 5) == -1)
		errExit("listen");

	printf("Listen will allocate address for it: ");
	len = sizeof(addr);
	if (getsockname(sfd, (struct sockaddr *)&addr, &len) == -1)
		errExit("getsockname");
	if (inet_ntop(AF_INET, &addr.sin_addr, addrstr, sizeof(addrstr)) == NULL)
		errExit("inet_ntop");
	printf("%s : %u\n", addrstr, ntohs(addr.sin_port));

	return 0;
}

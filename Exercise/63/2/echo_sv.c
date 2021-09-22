#include <arpa/inet.h>
#include <sys/socket.h>
#include <tlpi_hdr.h>

int main()
{
	int lfd, dfd; // listen fd and datagram fd
	struct sockaddr_in addr;

	// create a listening fd
	if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7); // port number uses 16 bits
	if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) == -1)
		errExit("inet_pton");

	if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("bind");
	if (listen(lfd, 5) == -1)
		errExit("listen");

	// bind a datagram socket fd
	if ((dfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		errExit("socket");

	if (bind(dfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("bind");

	// add these socket fds into select's rfdset list

	return 0;
}
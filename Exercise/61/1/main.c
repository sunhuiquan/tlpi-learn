#include <tlpi_hdr.h>
// #include "../../../tlpi-dist/lib/inet_sockets.h"
#include <inet_sockets.h>

int main()
{
	int listenFd;
	socklen_t len;

	listenFd = inetListen("50001", 5, &len);
	if (listenFd == -1)
		errExit("listenFd");

	sleep(10);

	return 0;
}
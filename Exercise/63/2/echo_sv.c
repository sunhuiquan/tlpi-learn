/* 一个使用I/O多路复用技术的并发服务器模型 */

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <tlpi_hdr.h>

#define BUF_SIZE 1024 // 可以通过调小这个缓冲区来更好地体现I/O多路复用的并发性

int main()
{
	int lfd, dfd; // listen fd and datagram fd
	struct sockaddr_in addr;
	struct sockaddr_in from_addr;
	socklen_t len;

	int nfds, nready;
	fd_set rfdset;

	int readn;
	char buf[BUF_SIZE];

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
	nfds = max(lfd, dfd) + 1;
	FD_ZERO(&rfdset);
	FD_SET(lfd, &rfdset);
	FD_SET(dfd, &rfdset);

	while (true)
	{
		nready = select(nfds, &rfdset, NULL, NULL, NULL);
		if (nready == -1)
			errExit("select");

		// 为了防止因为循环读不停到来的数据导致其他fd饥饿，我们每次只做适量的I/O操作
		if (FD_ISSET(lfd, &rfdset)) // can accept and create connected fd(s)
		{
			// to do

			--nready;
			if (!nready)
				continue;
		}

		if (FD_ISSET(dfd, &rfdset)) // can read the datagram fd
		{
			len = sizeof(from_addr);
			// 数据报socket read返回0可能是对端发来了空消息
			if ((readn = recvfrom(dfd, buf, BUF_SIZE, MSG_DONTWAIT,
								  (struct sockaddr *)&from_addr, &len)) == -1)
				errExit("recvfrom");

			if (sendto(dfd, buf, readn, MSG_DONTWAIT,
					   (struct sockaddr *)&from_addr, len) != readn)
				errExit("sendto");

			--nready;
			if (!nready)
				continue;
		}

		// for (int i = 0;;++i) // the rest of tcp connected fds
	}

	return 0;
}
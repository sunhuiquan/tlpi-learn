/* 一个使用I/O多路复用技术的并发服务器模型 */

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <tlpi_hdr.h>

#define BUF_SIZE 1024 // 可以通过调小这个缓冲区来更好地体现I/O多路复用的并发性
#define MAX_CONN_FD 1024

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

	int j;
	int connfd;
	int conn_fds[MAX_CONN_FD]; // 连接套接字的数据结构

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

	// initialize
	for (int i = 0; i < MAX_CONN_FD; ++i)
		conn_fds[i] = -1;

	// add these socket fds into select's rfdset list
	nfds = max(lfd, dfd) + 1;
	FD_ZERO(&rfdset);
	while (true)
	{
		FD_SET(lfd, &rfdset);
		FD_SET(dfd, &rfdset);
		for (int i = 0; i < MAX_CONN_FD; ++i)
			if (conn_fds[i] != -1)
				FD_SET(conn_fds[i], &rfdset);

		nready = select(nfds, &rfdset, NULL, NULL, NULL);
		if (nready == -1)
			errExit("select");

		// 为了防止因为循环读不停到来的数据导致其他fd饥饿，我们每次只做适量的I/O操作
		if (FD_ISSET(lfd, &rfdset)) // can accept and create connected fd(s)
		{
			for (j = 0; j < MAX_CONN_FD; ++j)
				if (conn_fds[j] == -1)
					break;
			if (j == MAX_CONN_FD)
				continue;

			if ((connfd = accept(lfd, NULL, NULL)) == -1)
				errExit("accept");
			if (connfd + 1 > nfds)
				nfds = connfd + 1;
			conn_fds[j] = connfd;

			--nready;
			if (!nready)
				continue;
		}

		if (FD_ISSET(dfd, &rfdset)) // can read the datagram fd
		{
			len = sizeof(from_addr);
			// 数据报socket read返回0可能是发来了空数据报
			if ((readn = recvfrom(dfd, buf, BUF_SIZE, MSG_DONTWAIT, (struct sockaddr *)&from_addr, &len)) == -1)
				errExit("recvfrom");

			if (sendto(dfd, buf, readn, MSG_DONTWAIT, (struct sockaddr *)&from_addr, len) != readn)
				errExit("sendto");

			--nready;
			if (!nready)
				continue;
		}

		for (int i = 0; i < MAX_CONN_FD; ++i) // the tcp connected fds
		{
			if (conn_fds[i] != -1 && FD_ISSET(conn_fds[i], &rfdset))
			{
				readn = read(conn_fds[i], buf, BUF_SIZE);
				if (readn < 0)
					errExit("read");
				else if (readn == 0)
				{
					// client shutdown or close will send FIN, and server will get EOF
					// 代表客户端(半)关闭，服务结束
					FD_CLR(conn_fds[i], &rfdset);
					close(conn_fds[i]);
					conn_fds[i] = -1;
					continue;
				}

				if (write(conn_fds[i], buf, readn) != readn)
					errExit("write");
			}
		}
	}

	return 0;
}
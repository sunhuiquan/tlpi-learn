#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <tlpi_hdr.h>

#define PORT_NUMBER 9990
#define MAXLINE 1024

int main(int argc, char *argv[])
{
	int sfd, readn, prior_sfd, lfd;
	struct sockaddr_in addr, addr2, addr3;
	socklen_t len;
	char buf[MAXLINE];

	// 1. 建立普通TCP连接
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NUMBER);
	if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) == -1)
		errExit("inet_pton");
	if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("connect");

	// 2. 建立优先TCP连接
	if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		errExit("socket");

	bzero(&addr2, sizeof(addr2));
	addr2.sin_family = AF_INET;
	addr2.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(lfd, (struct sockaddr *)&addr2, sizeof(addr2)) == -1)
		errExit("bind");

	if (listen(lfd, 10) == -1)
		errExit("listen");

	len = sizeof(addr3); // 获取listen()让内核分配的临时端口并发送
	if (getsockname(lfd, (struct sockaddr *)&addr3, &len) == -1)
		errExit("getsockname");

	if (write(sfd, addr3.sin_port, sizeof(addr3.sin_port)) != sizeof(addr3.sin_port))
		errExit("write");

	if ((prior_sfd = accept(lfd, NULL, NULL)) == -1)
		errExit("accept");

	close(lfd);

	if (write(sfd, "aaaaaaaaaa", 10) != 10)
		errExit("send");
	if (write(prior_sfd, "bbbbbbbbbb", 10) != 10)
		errExit("send");
	return 0;
}
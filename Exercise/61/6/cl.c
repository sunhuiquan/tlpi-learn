// #include <sys/socket.h>
#include <arpa/inet.h>
#include <tlpi_hdr.h>

#define PORT_NUMBER 9990
#define MAXLINE 1024

int main(int argc, char *argv[])
{
	int sfd, prior_sfd, lfd;
	struct sockaddr_in addr;
	socklen_t len;

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

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit("bind");

	if (listen(lfd, 10) == -1)
		errExit("listen");

	bzero(&addr, sizeof(addr)); // 反正bind或者connect用完这个结构就没用了，那就再重用下
	len = sizeof(addr);			// 获取listen()让内核分配的临时端口并发送
	if (getsockname(lfd, (struct sockaddr *)&addr, &len) == -1)
		errExit("getsockname");

	if (write(sfd, &addr.sin_port, sizeof(addr.sin_port)) != sizeof(addr.sin_port))
		errExit("write");

	if ((prior_sfd = accept(lfd, NULL, NULL)) == -1)
		errExit("accept");
	close(lfd); // because it is never userd anymore

	// 交错发送普通和优先数据
	if (write(sfd, "aaaaaaaaaa", 10) != 10)
		errExit("send");
	if (write(prior_sfd, "bbbbbbbbbb", 10) != 10)
		errExit("send");
	if (write(sfd, "aaaaaaaaaa", 10) != 10)
		errExit("send");
	if (write(prior_sfd, "bbbbbbbbbb", 10) != 10)
		errExit("send");
	if (write(sfd, "aaaaaaaaaa", 10) != 10)
		errExit("send");
	if (write(prior_sfd, "bbbbbbbbbb", 10) != 10)
		errExit("send");
	if (write(sfd, "aaaaaaaaaa", 10) != 10)
		errExit("send");
	if (write(prior_sfd, "bbbbbbbbbb", 10) != 10)
		errExit("send");
	return 0;
}
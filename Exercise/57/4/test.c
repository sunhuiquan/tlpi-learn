#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <tlpi_hdr.h>

int main()
{
	int a, b, c;
	struct sockaddr_un addr_a, addr_b;

	if ((a = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
		errExit("socket");

	if ((b = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
		errExit("socket");

	if ((c = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
		errExit("socket");

	if (unlink("/tmp/a") == -1 && errno != ENOENT)
		errExit("unlink");
	if (unlink("/tmp/b") == -1 && errno != ENOENT)
		errExit("unlink");

	memset(&addr_a, 0, sizeof(addr_a));
	addr_a.sun_family = AF_UNIX;
	strncpy(addr_a.sun_path, "/tmp/a", sizeof(addr_a.sun_path) - 1);
	if (bind(a, (struct sockaddr *)&addr_a, sizeof(struct sockaddr_un)) == -1)
		errExit("bind");

	memset(&addr_b, 0, sizeof(addr_b));
	addr_b.sun_family = AF_UNIX;
	strncpy(addr_b.sun_path, "/tmp/b", sizeof(addr_b.sun_path) - 1);
	if (bind(b, (struct sockaddr *)&addr_b, sizeof(struct sockaddr_un)) == -1)
		errExit("bind");

	if (connect(a, (struct sockaddr *)&addr_b, sizeof(struct sockaddr_un)) == -1)
		errExit("connect");

	if (bind(c, (struct sockaddr *)&addr_a, sizeof(struct sockaddr_un)) == -1)
		errExit("bind");

	return 0;
}
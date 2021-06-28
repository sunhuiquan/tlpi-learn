#include "myinet.h"

int inetConnect(const char *host, const char *service, int type)
{
	int ecode, cfd;
	struct addrinfo hints;
	struct addrinfo *res, *rp;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_canonname = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = type;

	ecode = getaddrinfo(host, service, &hints, &res);
	if (ecode != 0)
	{
		errno = ENOSYS;
		return -1;
	}

	// 使用rp是为了避免破坏res指向，后面free会用到res
	for (rp = res; rp; rp = rp->ai_next)
	{
		if ((cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
			continue;
		if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;
		close(cfd);
	}
	freeaddrinfo(res);

	return (rp == NULL) ? -1 : cfd;
}

int inetListen(const char *service, int backlog, socklen_t *addrlen)
{
}

int inetBind(const char *service, int type, socklen_t *addrlen)
{
}

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen,
					 char *addrStr, int addrStrLen)
{
}

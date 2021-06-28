#include "myinet.h"
#include <stdio.h>

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

int initPassiveSocket(const char *service, int type, socklen_t *addrlen, int dolisten, int backlog)
{
	int sfd;
	struct addrinfo hints;
	struct addrinfo *res, *rp;
	int optval;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = type;

	// getaddrinfo 错误返回错误码，不是-1，0表示正确
	if (getaddrinfo(NULL, service, &hints, &res) != 0)
		return -1;

	optval = 1;
	for (rp = res; rp; rp = rp->ai_next)
	{
		if ((sfd = socket(rp->ai_family, rp->ai_socktype, 0)) == -1)
			continue;

		if (dolisten)
		{
			if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
			{
				close(sfd);
				freeaddrinfo(res);
				return -1;
			}
		}

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;
		close(sfd);
	}
	freeaddrinfo(res);

	if (rp == NULL)
		return -1;

	if (dolisten)
	{
		if (listen(sfd, backlog) == -1)
			return -1;
	}

	if (addrlen != NULL)
		*addrlen = rp->ai_addrlen;
	return sfd;
}

int inetListen(const char *service, int backlog, socklen_t *addrlen)
{
	return initPassiveSocket(service, SOCK_STREAM, addrlen, 1, backlog);
}

int inetBind(const char *service, int type, socklen_t *addrlen)
{
	return initPassiveSocket(service, type, addrlen, 0, 0);
}

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen)
{
	char host[NI_MAXHOST], service[NI_MAXSERV];

	if (getnameinfo(addr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV) == 0)
		snprintf(addrStr, addrStrLen, "(%s,%s)", host, service);
	else
		snprintf(addrStr, addrStrLen, "(?UNKNOWN?)");
	return addrStr;
}

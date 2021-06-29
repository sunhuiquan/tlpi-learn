#include <tlpi_hdr.h>
#include "header/kv.h"
#include "header/myinet.h"
#include "../../../tlpi-dist/sockets/read_line.h"

#define BACKLOG 5
#define MAXLINE 1024

kv_dsa *kv;

int handle_command(char *command, char *result);

int main()
{
	int svfd, confd;
	struct sockaddr_storage claddr;
	socklen_t addrlen;
	char command[MAXLINE];
	char bufline[MAXLINE];
	ssize_t n;
	char addrStr[IS_ADDR_STR_LEN];

	if ((kv = init_kv()) == NULL)
		errExit("init_kv");

	if ((svfd = inetListen("50000", BACKLOG, NULL)) == -1)
		errExit("inetListen");

	for (;;)
	{
		addrlen = sizeof(struct sockaddr_storage);
		if ((confd = accept(svfd, (struct sockaddr *)&claddr, &addrlen)) == -1)
		{
			errMsg("accept fail");
			continue;
		}
		printf("connect from: %s\n", inetAddressStr((struct sockaddr *)&claddr,
													addrlen, addrStr, IS_ADDR_STR_LEN));

		if ((n = readLine(confd, command, MAXLINE)) <= 0)
		{
			errMsg("readLine fail");
			close(confd);
			continue;
		}
		printf("command: %s", command);

		handle_command(command, bufline);

		strcpy(bufline, command);

		if (write(confd, bufline, n) != n)
			errMsg("write fail");
		close(confd);
	}

	return 0;
}

int handle_command(char *command, char *result)
{
	char temp[MAXLINE];
	strncpy(temp, command, MAXLINE);
	temp[strlen(temp) - 1] = ' '; // '\n' 变 ' '
	char *words[4];
	char *ptemp = temp, *p;

	int i = 0;
	for (; i < 3; ++i)
	{
		p = strchr(ptemp, ' ');
		if (p == NULL)
			break;

		*p = '\0';
		words[i] = ptemp;
		ptemp = p + 1;
	}

	if (!strcmp(words[0], "add") && i == 3)
		return add_kv(kv, words[1], words[2]);
	else if (!strcmp(words[0], "delete") && i == 2)
		return delete_kv(kv, words[1]);
	else if (!strcmp(words[0], "change") && i == 3)
		return change_kv(kv, words[1], words[2]);
	else if (!strcmp(words[0], "show") && i == 1)
	{
		kv_dsa *p = kv;
		for (; p; p = p->next)
			sprintf(result, "(%s : %s)\n", p->key, p->value);
		return 0;
	}

	sprintf(result, "invalid command\n");
	return -1;
}

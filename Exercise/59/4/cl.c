#include <tlpi_hdr.h>
#include "header/kv.h"
#include "header/myinet.h"
#include "../../../tlpi-dist/sockets/read_line.h"

#define MAXLINE 1024

int main(int argc, char *argv[])
{
	int clfd;
	char bufline[MAXLINE];
	ssize_t n;
	char command[MAXLINE];

	if (argc != 3)
		printf("usage: <host> <port>");

	if ((clfd = inetConnect(argv[1], argv[2], SOCK_STREAM)) == -1)
		errExit("inetConnect");

	for (;;)
	{
		printf("command:\n\t1.add <key> <value>\n\t2.delete <key>\n\t"
			   "3.change <key> <value>\n\t4.show\n\t5.quit\n");

		n = readLine(STDIN_FILENO, command, MAXLINE);
		if (n < 0)
			errExit("readLine");
		else if (n == 0)
			continue;

		if (!strncmp(command, "add", 3) || !strncmp(command, "delete", 6) ||
			!strncmp(command, "change", 6) || !strncmp(command, "show", 4))
		{
			if (write(clfd, command, strlen(command)) != strlen(command)) // 不含'\0'
				errExit("write");
		}
		else if (!strncmp(command, "quit", 4))
		{
			break;
		}
		else
		{
			printf("wrong input, repeat again\n");
			continue;
		}

		if ((n = readLine(clfd, bufline, MAXLINE)) < 0)
			errExit("readLine");
		if (n)
			printf("%s", bufline);
	}

	return 0;
}
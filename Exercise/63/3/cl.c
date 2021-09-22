#include <tlpi_hdr.h>

#define MAXLINE 1024

int main()
{
	int readn;
	char buf[MAXLINE];

	sleep(5);
	if ((readn = read(STDIN_FILENO, buf, MAXLINE)) == -1)
		errExit("read");
	printf("------------%d----------------\n", readn);
	for (int i = 0; i < readn; ++i)
		if (write(STDOUT_FILENO, &buf[i], 1) != 1)
			errExit("write");

	return 0;
}
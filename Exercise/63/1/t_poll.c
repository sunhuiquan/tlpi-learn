#include <sys/poll.h>
#include <tlpi_hdr.h>

#define MAXLINE 1024

static void
usageError(const char *progName)
{
	fprintf(stderr, "Usage: %s {timeout(ms)|-} fd-num[rw]...\n", progName);
	fprintf(stderr, "    - means infinite timeout; \n");
	fprintf(stderr, "    r = monitor for read\n");
	fprintf(stderr, "    w = monitor for write\n\n");
	fprintf(stderr, "    e.g.: %s - 0rw 1w\n", progName);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int ready, nfds, fd, numRead, j, readn, msecs;
	struct pollfd *ppfd;
	char buf[10]; /* Large enough to hold "rw\0" */
	char buf2[MAXLINE];

	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageError(argv[0]);

	if (strcmp(argv[1], "-") == 0)
		msecs = -1;
	else
		msecs = getLong(argv[1], 0, "timeout");

	nfds = argc - 2;
	if ((ppfd = calloc(nfds, sizeof(struct pollfd))) == NULL)
		errExit("calloc");

	for (j = 2; j < argc; j++)
	{
		numRead = sscanf(argv[j], "%d%2[rw]", &fd, buf);
		if (numRead != 2)
			usageError(argv[0]);

		ppfd[j - 2].fd = fd;
		if (strchr(buf, 'r') != NULL)
			ppfd[j - 2].events = POLLIN;
		if (strchr(buf, 'w') != NULL)
			ppfd[j - 2].events |= POLLOUT;
	}

	ready = poll(ppfd, nfds, msecs);
	if (ready == -1)
		errExit("poll");

	printf("ready = %d\n", ready);
	for (int i = 0; i < nfds; ++i)
	{
		if (ppfd[i].revents & POLLIN)
		{
			printf("get POLLIN: \n");
			readn = read(ppfd[i].fd, buf2, MAXLINE);
			if (readn < 0)
				errExit("read");
			else if (readn == 0)
				ppfd[i].fd = -1;

			if (write(ppfd[i].fd, buf2, readn) != readn)
				errExit("write");
		}
		else
		{
			printf("get POLLERR | POLLHUP | POLLNVAL\n");
			ppfd[i].fd = -1;
		}
	}

	exit(EXIT_SUCCESS);
}

#include <sys/time.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <signal.h>
#include "tlpi_hdr.h"

static int pfd[2]; /* File descriptors for pipe */

static void
handler(int sig)
{
	int savedErrno; /* In case we change 'errno' */

	savedErrno = errno;
	if (write(pfd[1], "x", 1) == -1 && errno != EAGAIN)
		errExit("write");
	errno = savedErrno;
}

int main(int argc, char *argv[])
{
	struct pollfd *ppfd;
	int ready, nfds, flags;
	int timeout;
	struct sigaction sa;
	char ch;
	int fd, index, j;

	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s {timeout(ms)|-} fd...\n"
				 "\t\t('-' means infinite timeout)\n",
				 argv[0]);

	/* Initialize 'timeout', 'readfds', and 'nfds' for select() */

	if (strcmp(argv[1], "-") == 0)
		timeout = -1; /* Infinite timeout */
	else
		timeout = getInt(argv[1], 0, "timeout");

	nfds = argc - 2 + 1;						// add a pipe's read-end fd
	ppfd = calloc(nfds, sizeof(struct pollfd)); // 如果是没用完位置的，要初始化-1，因为0也是fd
	if (ppfd == NULL)
		errExit("calloc");

	for (j = 2, index = 0; j < argc; ++j, ++index)
	{
		fd = getInt(argv[j], 0, "fd");
		ppfd[index].fd = fd;
		ppfd[index].events = POLLIN;
	}

	if (pipe(pfd) == -1)
		errExit("pipe");

	// 最后index是pfd[0]所在的ppfd的下标
	ppfd[index].fd = pfd[0];
	ppfd[index].events = POLLIN;

	/* Make read and write ends of pipe nonblocking */
	/* 因为这是一个self-pipe指代的是信号边缘触发的情况，所以要非阻塞 */
	flags = fcntl(pfd[0], F_GETFL);
	if (flags == -1)
		errExit("fcntl-F_GETFL");
	flags |= O_NONBLOCK; /* Make read end nonblocking */
	if (fcntl(pfd[0], F_SETFL, flags) == -1)
		errExit("fcntl-F_SETFL");

	flags = fcntl(pfd[1], F_GETFL);
	if (flags == -1)
		errExit("fcntl-F_GETFL");
	flags |= O_NONBLOCK; /* Make write end nonblocking */
	// 对应太多信号到来导致pipe缓冲区溢出，避免这种情况阻塞,该程序这种情况会死锁
	if (fcntl(pfd[1], F_SETFL, flags) == -1)
		errExit("fcntl-F_SETFL");

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART; /* Restart interrupted reads()s */
	sa.sa_handler = handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		errExit("sigaction");

	while ((ready = poll(ppfd, nfds, timeout)) == -1 && errno == EINTR)
		continue;
	if (ready == -1) /* Unexpected error */
		errExit("select");

	if (ppfd[index].revents & POLLIN)
	{ /* Handler was called */
		printf("A signal was caught\n");

		for (;;)
		{ /* Consume bytes from pipe */
			if (read(pfd[0], &ch, 1) == -1)
			{
				if (errno == EAGAIN)
					break; /* No more bytes */
				else
					errExit("read"); /* Some other error */
			}

			/* Perform any actions that should be taken in response to signal */
		}
	}

	printf("ready = %d\n", ready);
	for (int i = 0; i < index; ++i)
		printf("%d %s\n", ppfd[i].fd, (ppfd[i].revents & POLLIN) ? "r" : "-");

	printf("%d: %s   (read end of pipe)\n", ppfd[index].fd,
		   (ppfd[index].revents & POLLIN) ? "r" : "-");

	exit(EXIT_SUCCESS);
}

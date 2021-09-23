#include <sys/epoll.h>
#include <tlpi_hdr.h>

#define MAXPIPE 32
#define MAX_EVENTS 2 // 用这个小的来体现

int main(int argc, char *argv[])
{
	int arr_pfd[MAXPIPE][2];
	int efd, num, ready;
	pid_t pid;
	struct epoll_event ev;
	struct epoll_event evlist[MAX_EVENTS];

	if (argc != 2)
	{
		printf("%s usage: [pipe-num]\n", argv[0]);
		printf("^c to quit\n");
		exit(EXIT_SUCCESS);
	}

	if (sscanf(argv[1], "%d", &num) != 1 || num == -1)
		errExit("sscanf");
	if (num > MAXPIPE)
	{
		printf("too many\n");
		exit(EXIT_SUCCESS);
	}
	printf("num %d\n", num);

	for (int i = 0; i < num; ++i)
	{
		if (pipe(arr_pfd[i]) == -1)
			errExit("pipe");
	}

	switch (pid = fork())
	{
	case -1:
		errExit("fork");
		break;

	case 0:
		// child
		// 这里用不着不停写数据，只要我们epoll存在就绪返回之后不读数据，那么下一次就绪的仍然是就绪
		for (int i = 0; i < num; ++i)
		{
			close(arr_pfd[i][0]);
			if (write(arr_pfd[i][1], "a", 1) != 1)
				errExit("write");
		}
		_exit(EXIT_SUCCESS);
		break;

	default:
		break;
	}

	// parent
	sleep(2);							 // wait for child finishing write data
	if ((efd = epoll_create(num)) == -1) // fork之后create，不然子进程也有个指向同一个打开描述的efd
		errExit("epoll_create");

	for (int i = 0; i < num; ++i)
	{
		close(arr_pfd[i][1]);
		ev.events = EPOLLIN;
		ev.data.fd = arr_pfd[i][0];
		if (epoll_ctl(efd, EPOLL_CTL_ADD, arr_pfd[i][0], &ev) == -1)
			errExit("epoll_ctl");
	}

	for (int times = 0; times < 30; ++times)
	{
		ready = epoll_wait(efd, evlist, MAX_EVENTS, -1);
		printf("--------------\n");
		if (ready == -1)
			errExit("epoll_wait");

		for (int i = 0; i < ready; ++i)
		{
			printf("fd=%d; events: %s%s%s\n", evlist[i].data.fd,
				   (evlist[i].events & EPOLLIN) ? "EPOLLIN " : "",
				   (evlist[i].events & EPOLLHUP) ? "EPOLLHUP " : "",
				   (evlist[i].events & EPOLLERR) ? "EPOLLERR " : "");
		}
	}

	return 0;
}
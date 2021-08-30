#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <tlpi_hdr.h>

int main()
{
	int fd;
	pid_t pid;

	if ((fd = open("./file", O_RDONLY)) == -1)
		errExit("open");

	if (flock(fd, LOCK_EX) == -1)
	{
		if (errno == EWOULDBLOCK)
			printf("already locked");
		else
			errExit("flock");
	}
	else
		printf("Parent: %ld 获取 flock 互斥锁\n", (long)getpid());

	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	case 0: // child
		sleep(3);
		printf("Child: %ld 释放 flock 互斥锁并终止\n", (long)getpid());
		close(fd); // 关闭任意一个fd就会释放flock
		_exit(EXIT_SUCCESS);
		break;

	default: // parent
		break;
	}

	wait(NULL);
	printf("wait等到子进程结束并休眠5s\n");
	return 0;
}
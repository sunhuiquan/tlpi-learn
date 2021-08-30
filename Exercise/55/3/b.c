#include <fcntl.h>
#include <sys/file.h>
#include <tlpi_hdr.h>

int main()
{
	int fd;

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
		printf("PID: %ld 获取 flock 互斥锁\n", (long)getpid());

	if (flock(fd, LOCK_UN) == -1)
		errExit("flock");
	printf("PID: %ld 释放 flock 互斥锁\n", (long)getpid());

	return 0;
}
#include <fcntl.h>
#include <sys/file.h>
#include <tlpi_hdr.h>

int main()
{
	int fd;
	struct flock fl;

	if ((fd = open("./file", O_RDWR)) == -1)
		errExit("open");

	if (flock(fd, LOCK_EX) == -1)
	{
		if (errno == EWOULDBLOCK)
			printf("already locked");
		else
			errExit("flock");
	}
	else
		printf("获取 flock 互斥锁\n");

	fl.l_len = 0;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_type = F_WRLCK;

	if (fcntl(fd, F_SETLK, &fl) == -1)
	{
		if (errno == EAGAIN || errno == EACCES)
			printf("already locked");
		else if (errno == EDEADLK)
			printf("dead lock");
		else
			errExit("fcntl");
	}
	else
		printf("获取同一个文件的 fcntl 的写记录锁(并没有释放 flock 互斥锁)\n");

	close(fd); // 关闭fd自动释放关联的flock锁
	return 0;  // 进程结束后自动释放fcntl锁
}

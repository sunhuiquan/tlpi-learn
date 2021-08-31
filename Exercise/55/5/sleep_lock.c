#include <fcntl.h>
#include <sys/file.h>
#include <tlpi_hdr.h>

int main()
{
	int fd;
	struct flock fl;

	if ((fd = open("./file", O_RDWR)) == -1)
		errExit("open");

	for (int i = 0; i < 40001; ++i)
	{
		fl.l_len = 1;
		fl.l_start = i * 2;
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
	}

	printf("休眠开始\n");
	sleep(1000);

	close(fd); // fcntl的释放语义是关掉任何一个对应的fd(无所谓是否还有其他同指向fd)就会释放记录锁
	return 0;
}

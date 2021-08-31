#include <fcntl.h>
#include <sys/file.h>
#include <string.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
	int fd, n;
	struct flock fl;

	n = atoi(argv[1]);
	if ((fd = open("./file", O_RDWR)) == -1)
		errExit("open");

	for (int i = 0; i < 10000; ++i)
	{
		fl.l_len = 1;
		fl.l_start = n * 2;
		fl.l_whence = SEEK_SET;
		fl.l_type = F_WRLCK;

		if (fcntl(fd, F_SETLK, &fl) == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				if (errno == EDEADLK)
					printf("dead lock");
				else
					errExit("fcntl");
			}
		}
		else
			printf("You should lock first to run the time test.\n");
	}

	close(fd); // fcntl的释放语义是关掉任何一个对应的fd(无所谓是否还有其他同指向fd)就会释放记录锁
	return 0;
}
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1000

char *currTime(const char *format);

int main(int argc, char *argv[])
{
	int fd_a, fd_b;
	struct flock fl;

	fd_a = open("./a", O_RDWR);
	if (fd_a == -1)
		errExit("open");

	fd_b = open("./b", O_RDWR);
	if (fd_b == -1)
		errExit("open");

	fl.l_len = 0;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_type = F_WRLCK;

	printf("PID %ld: requesting file a at %s\n", (long)getpid(),
		   currTime("%T"));
	if (fcntl(fd_a, F_SETLKW, &fl) == -1)
	{
		if (errno == EAGAIN || errno == EACCES)
			printf("already locked");
		else if (errno == EDEADLK)
			printf("dead lock");
		else
			errExit("fcntl");
	}
	printf("PID %ld: have got write lock a at %s\n", (long)getpid(), currTime("%T"));
	sleep(3);

	printf("write强制性下要无别的进程有b的记录锁，但write本身不持有锁\n");
	if (write(fd_b, "b", 1) != 1) // 强制性锁让write会阻塞到无别的进程持有写锁的时候
		errExit("write");
	printf("write b成功\n");
	sleep(3);

	fl.l_type = F_UNLCK;
	if (fcntl(fd_a, F_SETLK, &fl) == -1)
		errExit("fcntl");
	else
		printf("PID %ld: have released write lock a at %s\n", (long)getpid(), currTime("%T"));

	exit(EXIT_SUCCESS);
}

char *currTime(const char *format)
{
	static char buf[BUF_SIZE]; /* Nonreentrant */
	time_t t;
	size_t s;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	if (tm == NULL)
		return NULL;

	s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);

	return (s == 0) ? NULL : buf;
}

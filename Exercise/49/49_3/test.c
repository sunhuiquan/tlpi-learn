#include <tlpi_hdr.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

void sigbus_hander(int sig)
{
	printf("get SIGBUS\n");
	exit(EXIT_SUCCESS);
}

int main()
{
	int fd;
	void *addr;
	struct stat buf;
	struct sigaction act;

	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = sigbus_hander;
	if (sigaction(SIGBUS, &act, NULL) == -1)
		errExit("sigaction");

	if ((fd = open("a.txt", O_RDONLY)) == -1)
		errExit("open");
	if (fstat(fd, &buf) == -1)
		errExit("fstat");
	printf("size: %lu\n", buf.st_size);
	if ((addr = mmap(NULL, 8192, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
		errExit("mmap");
	if (close(fd) == -1)
		errExit("close");

	printf("normal\n");
	printf("%c\n", *((char *)addr + 100));

	printf("sigbus\n");
	printf("%c\n", *((char *)addr + buf.st_size));

	printf("sigsev\n");
	printf("%c\n", *((char *)addr + 5000));

	return 0;
}
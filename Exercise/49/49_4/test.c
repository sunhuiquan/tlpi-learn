#include <sys/mman.h>
#include <tlpi_hdr.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
	int fd;
	void *addr;
	struct stat buf;
	char temp[10000];
	long pgsize;

	pgsize = sysconf(_SC_PAGESIZE);
	if ((fd = open("a.txt", O_RDONLY)) == -1)
		errExit("open");
	if (fstat(fd, &buf) == -1)
		errExit("fstat");
	if ((addr = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		errExit("mmap");

	printf("before:\n");
	memcpy(temp, addr, 2 * pgsize);
	temp[2 * pgsize] = '\0';
	printf("%s", temp);

	if ((mmap(addr + pgsize, pgsize, PROT_READ, MAP_PRIVATE | MAP_FIXED, fd, 0)) == MAP_FAILED)
		errExit("mmap");
	if ((mmap(addr, pgsize, PROT_READ, MAP_PRIVATE | MAP_FIXED, fd, pgsize)) == MAP_FAILED)
		errExit("mmap");

	printf("after:\n");
	memcpy(temp, addr, 2 * pgsize);
	temp[2 * pgsize] = '\0';
	printf("%s", temp);

	if (close(fd) == -1)
		errExit("close");

	return 0;
}
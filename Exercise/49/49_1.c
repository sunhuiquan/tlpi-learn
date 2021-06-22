#include <tlpi_hdr.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int source_fd, dst_fd;
	void *source_addr, *dst_addr;
	struct stat buf;

	if (argc != 3)
	{
		printf("%s usage: source-file destinate-file\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	if ((source_fd = open(argv[1], O_RDONLY)) == -1)
		errExit("open");
	if (fstat(source_fd, &buf) == -1)
		errExit("fstat");
	if (buf.st_size == 0)
		exit(EXIT_SUCCESS);

	if ((source_addr = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, source_fd, 0)) == MAP_FAILED)
		errExit("mmap");
	if (close(source_fd) == -1)
		errExit("close");

	if ((dst_fd = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0666)) == -1)
		errExit("open");
	if (ftruncate(dst_fd, buf.st_size) == -1)
		errExit("ftruncate");
	if ((dst_addr = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, dst_fd, 0)) == MAP_FAILED)
		errExit("mmap");
	if (close(dst_fd) == -1)
		errExit("close");

	memcpy(dst_addr, source_addr, buf.st_size);

	if (msync(dst_addr, buf.st_size, MS_SYNC) == -1)
		errExit("msync");

	if (munmap(source_addr, buf.st_size) == -1)
		errExit("munmap");
	if (munmap(dst_addr, buf.st_size) == -1)
		errExit("munmap");

	return 0;
}
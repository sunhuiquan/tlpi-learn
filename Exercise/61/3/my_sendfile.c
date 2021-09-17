/* An easy approach to achieve sendfile() */

#include <fcntl.h>
#include <tlpi_hdr.h>

ssize_t my_sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

int main()
{
	int ifd, ofd1, ofd2;
	off_t off = 2;

	if ((ifd = open("infile", O_RDONLY)) == -1)
		errExit("open");
	if ((ofd1 = open("outfile1", O_TRUNC | O_CREAT | O_RDWR, 0666)) == -1)
		errExit("open");
	if ((ofd2 = open("outfile2", O_TRUNC | O_CREAT | O_RDWR, 0666)) == -1)
		errExit("open");

	if (my_sendfile(ofd1, ifd, &off, 2) < 0) // 11
		errExit("my_sendfile");
	printf("before read 2 off is %ld, then off is %ld\n", (long)off, (long)off);
	if (my_sendfile(ofd2, ifd, NULL, 1024) < 0) // 从当前打开文件状态种的偏移量开始(4)，遇到'\0'停下
		errExit("my_sendfile");

	return 0;
}

#define MAXLINE 1024
ssize_t my_sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
	int readn, rest = count;
	static char buf[MAXLINE];
	if (offset && lseek(in_fd, *offset, SEEK_SET) == -1)
		return -1;

	while ((readn = read(in_fd, buf, (rest > MAXLINE) ? MAXLINE : rest)) > 0)
	{
		rest -= readn;
		if (write(out_fd, buf, readn) != readn)
			return -1;
		if (rest == 0)
			break;
	}
	if (readn == -1)
		return -1;

	if (offset)
		*offset += (count - rest);
	return (count - rest);
}
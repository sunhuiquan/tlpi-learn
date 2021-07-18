/** 
 * tail [-n num] file
 * using lseek(), read(), write() etc. 
 */

#include <tlpi_hdr.h>
#include <fcntl.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
	int num = 10, ret_num = 0;
	int fd;
	char buf[BUFSIZE];

	if (argc == 1 || argc > 3 || argv[1][2] == 'h')
	{
		printf("usage: tail [num] file\n");
		exit(EXIT_SUCCESS);
	}
	if (argc > 2)
		num = atoi(argv[1]);

	if ((fd = open(argv[argc - 1], O_RDONLY)) == -1)
		errExit("open");

	if (lseek(fd, 0, SEEK_END) == -1)
		errExit("lseek");
	while (1)
	{
		long cur = lseek(fd, 0, SEEK_CUR);
		if (cur >= BUFSIZE)
		{
			if (lseek(fd, -BUFSIZE, SEEK_CUR) == -1)
				errExit("lseek");
			if (read(fd, buf, BUFSIZE) != BUFSIZE)
				errExit("read");
			if (lseek(fd, -BUFSIZE, SEEK_CUR) == -1)
				errExit("lseek");
			for (int i = BUFSIZE - 1; i >= 0; --i)
			{
				if (buf[i] == '\n')
				{
					++ret_num;
					if (ret_num == num)
					{
						lseek(fd, i + 1, SEEK_CUR);
						break;
					}
				}
			}
			if (ret_num == num)
				break;
		}
		else
		{
			if (lseek(fd, 0, SEEK_SET) == -1)
				errExit("lseek");
			if (read(fd, buf, cur) != cur)
				errExit("read");
			if (lseek(fd, 0, SEEK_SET) == -1)
				errExit("lseek");
			for (int i = cur - 1; i >= 0; --i)
			{
				if (buf[i] == '\n')
				{
					++ret_num;
					if (ret_num == num)
					{
						lseek(fd, i + 1, SEEK_CUR);
						break;
					}
				}
			}
			break;
		}
	}

	ssize_t n;
	while ((n = read(fd, buf, BUFSIZE)) > 0)
	{
		if (write(STDOUT_FILENO, buf, n) != n)
			errExit("write");
	}
	if (n == -1)
		errExit("read");

	close(fd);
	return 0;
}
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

char *mygetcwd(char *buf, size_t size) // size含着'\0'
{
	struct stat sbuf;
	int origin_fd;
	DIR *dir;
	struct dirent *dirent;
	unsigned long ino;
	unsigned long dev;
	char *p;
	char retemp[PATH_MAX] = "/"; // 方便最后处理字符串

	if ((origin_fd = open(".", O_RDONLY)) == -1) // 为了以后能返回最初目录
		return NULL;
	if (stat(".", &sbuf) == -1)
		return NULL;
	ino = sbuf.st_ino;
	dev = sbuf.st_dev; // 为了确认是同一个文件系统(因为不同的FS的相同ino指向的完全不同文件)

	while (1)
	{
		if (chdir("..") == -1)
			return NULL;

		if (stat(".", &sbuf) == -1)
		{
			fchdir(origin_fd);
			return NULL;
		}
		if (ino == sbuf.st_ino && dev == sbuf.st_dev) // 利用只有根目录的父目录是本身
			break;

		if (!(dir = opendir(".")))
		{
			fchdir(origin_fd);
			return NULL;
		}

		while (1)
		{
			errno = 0;
			if (!(dirent = readdir(dir)))
				break;

			if (ino == dirent->d_ino)
			{
				if (stat(dirent->d_name, &sbuf) == -1)
				{
					fchdir(origin_fd);
					return NULL;
				}

				if (dev == sbuf.st_dev)
				{
					strcat(retemp, dirent->d_name);
					strcat(retemp, "/"); // 上一次已经加了'/',所以根目录可以直接break
					break;
				}
			}
		}
		if (errno || !dirent)
		{
			fchdir(origin_fd);
			return NULL;
		}

		if (stat(".", &sbuf) == -1)
		{
			fchdir(origin_fd);
			return NULL;
		}
		ino = sbuf.st_ino;
		dev = sbuf.st_dev;
	}

	fchdir(origin_fd);
	sprintf(buf, "/");
	retemp[strlen(retemp) - 1] = '\0';
	while (1)
	{
		if ((p = strrchr(retemp, '/')) == NULL)
			break;
		strcat(buf, p + 1);
		strcat(buf, "/");
		*p = '\0';
	}
	buf[strlen(buf) - 1] = '\0';
	return buf;
}

int main()
{
	char buf[PATH_MAX];
	char buf2[PATH_MAX];
	printf("getcwd:   %s\n", getcwd(buf, PATH_MAX) ? buf : "getcwd failed");
	printf("mygetcwd: %s\n", mygetcwd(buf2, PATH_MAX) ? buf2 : "getcwd failed");

	return 0;
}
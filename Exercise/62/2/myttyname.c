#define _XOPEN_SOURCE 600

#include <unistd.h>
#include <sys/stat.h>
#include <ftw.h>
#include <string.h>
#include <stdio.h>

#define MAXNAME 1024

static struct stat st;
static char myttyname[MAXNAME];
static int find;

static int find_ttyname(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf)
{
	if (ftwbuf->level != 1)
		return 0;

	struct stat lst;
	if (lstat(pathname, &lst) == -1) // 为了识别出链接
		return -1;

	if (!S_ISLNK(lst.st_mode) && S_ISCHR(lst.st_mode) && lst.st_rdev == st.st_rdev)
	{
		// printf("%s\n", pathname);
		strncpy(myttyname, pathname, MAXNAME);
		find = 1;
	}
	return 0;
}

char *my_ttyname(int fd)
{
	if (isatty(fd) == 0)
		return NULL;

	if (fstat(fd, &st) == -1)
		return NULL;

	find = 0;
	if (nftw("/dev", find_ttyname, 10, 0) == -1)
		return NULL;

	if (!find && nftw("/dev/pts", find_ttyname, 10, 0) == -1)
		return NULL;

	if (find)
		return myttyname;
	return NULL;
}
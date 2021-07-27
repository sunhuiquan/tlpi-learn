#define _XOPEN_SOURCE 600
#include <tlpi_hdr.h>
#include <ftw.h>

int mynftw(const char *__dir, __nftw_func_t __func, int __descriptors, int __flag);

static int regular_file = 0;
static int dir_file = 0;
static int sym_link = 0;

int count_file(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf)
{
	// printf("%s\n", pathname);
	if (S_ISREG(statbuf->st_mode))
		++regular_file;
	if (S_ISDIR(statbuf->st_mode))
		++dir_file;
	if (S_ISLNK(statbuf->st_mode))
		++sym_link;
	return 0;
}

int main(int argc, char *argv[])
{
	int flag = 0;
	if (argc != 2 || !strcmp(argv[1], "--help"))
	{
		printf("usage: dir\n");
		exit(EXIT_SUCCESS);
	}

	flag |= FTW_PHYS; // 不对符号链接解引用，才可以看到符号链接本身
	if (mynftw(argv[1], count_file, 30, flag) == -1)
		errExit("nftw");

	printf("Regular files takes about %.4lf%%\n", (double)regular_file / (regular_file + dir_file + sym_link) * 100);
	printf("Dirent  files takes about %.4lf%%\n", (double)dir_file / (regular_file + dir_file + sym_link) * 100);
	printf("symbol  links takes about %.4lf%%\n", (double)sym_link / (regular_file + dir_file + sym_link) * 100);

	return 0;
}
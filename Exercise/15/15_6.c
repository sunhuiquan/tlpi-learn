/* 
 * do the same thing as chmod a+rX file
 * 三个级别权限都加上r，然后如果是文件，那么三个级别有一个有x就都加上x，如果是目录则都加上x
 * 只使用stat() and chmod() 
 */

#include <tlpi_hdr.h>
#include <sys/stat.h>

int do_chmod_rx(const char *str)
{
	struct stat st;
	mode_t mode;

	if (stat(str, &st) == -1)
		return -1;

	mode = st.st_mode;
	mode = mode | S_IRUSR | S_IRGRP | S_IROTH;

	if (S_ISDIR(st.st_mode))
		mode = mode | S_IXUSR | S_IXGRP | S_IXOTH;
	else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
		mode = mode | S_IXUSR | S_IXGRP | S_IXOTH;

	return chmod(str, mode);
}

int main(int argc, char *argv[])
{
	if (argc == 1 || strcmp(argv[1], "--help") == 0)
	{
		printf("usage <file> [files...]\n");
		exit(EXIT_SUCCESS);
	}

	for (int i = 1; i < argc; ++i)
		if (do_chmod_rx(argv[i]) == -1)
			errExit("do_chmod_rx");

	return 0;
}
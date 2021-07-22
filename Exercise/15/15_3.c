#include <sys/stat.h>
#include <time.h>
#include "tlpi_hdr.h"

static void
displayStatTime(const struct stat *sb)
{
	printf("Last file access:         %lf\n", (double)(1000000000 * sb->st_atim.tv_sec + sb->st_atim.tv_nsec) / 1000000000);
	printf("Last file modification:   %lf\n", (double)(1000000000 * sb->st_mtim.tv_sec + sb->st_mtim.tv_nsec) / 1000000000);
	printf("Last status change:       %lf\n", (double)(1000000000 * sb->st_ctim.tv_sec + sb->st_ctim.tv_nsec) / 1000000000);
}
int main(int argc, char *argv[])
{
	struct stat sb;
	Boolean statLink; /* True if "-l" specified (i.e., use lstat) */
	int fname;		  /* Location of filename argument in argv[] */

	statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;
	/* Simple parsing for "-l" */
	fname = statLink ? 2 : 1;

	if (fname >= argc || (argc > 1 && strcmp(argv[1], "--help") == 0))
		usageErr("%s [-l] file\n"
				 "        -l = use lstat() instead of stat()\n",
				 argv[0]);

	if (statLink)
	{
		if (lstat(argv[fname], &sb) == -1)
			errExit("lstat");
	}
	else
	{
		if (stat(argv[fname], &sb) == -1)
			errExit("stat");
	}

	displayStatTime(&sb);

	exit(EXIT_SUCCESS);
}

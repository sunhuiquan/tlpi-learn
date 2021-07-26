#include <tlpi_hdr.h>
#include <limits.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/stat.h>

#define MAXBUF PATH_MAX

char *myrealpath(const char *name, char *resolved)
{
	struct stat sbuf;
	char str[PATH_MAX];
	int old_fd, fd;

	if (stat(name, &sbuf) == -1)
		return NULL;

	if ((old_fd = open(".", O_RDONLY)) == -1)
		return NULL;
	snprintf(str, PATH_MAX, "%s", name);
	if ((fd = open(dirname(str), O_RDONLY)) == -1)
		return NULL;

	if (fchdir(fd) == -1)
		return NULL;
	if (!getcwd(resolved, PATH_MAX))
		return NULL;
	strcat(resolved, "/");
	strcat(resolved, name);

	if (fchdir(old_fd) == -1)
		return NULL;

	close(fd);
	close(old_fd);

	return resolved;
}

int main(int argc, char *argv[])
{
	char buf[MAXBUF];

	if (argc != 2 || !strcmp(argv[1], "--help"))
	{
		printf("usage: path\n");
		exit(EXIT_SUCCESS);
	}

	if (!myrealpath(argv[1], buf))
		errExit("myrealpath");
	printf("%s --> %s\n", argv[1], buf);

	return 0;
}
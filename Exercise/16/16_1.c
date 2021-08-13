#include <sys/xattr.h>
#include <errno.h>
#include <tlpi_hdr.h>

#define MAX_PATH 4096
#define MAX_VALUE 8192

void print_usage()
{
	printf("Usage: setfattr {-n name} [-v value] [-h] file...\n"
		   "       setfattr {-x name} [-h] file...");
}

/* command format:
 * setfattr [-h] -n name [-v value] pathname...
 * setfattr [-h] -x name pathname...
 */
int my_setfattr(int argc, char *argv[])
{
	if (argc < 1)
		return -1;
	if (strcmp(argv[0], "setfattr"))
		return -1;

	int opt = 0;
	int not_follow = 0, has_n = 0, has_v = 0, has_x = 0;
	char name[MAX_PATH];
	char value[MAX_VALUE] = "";

	while ((opt = getopt(argc, argv, "hn:v:x:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			not_follow = 1;
			break;
		case 'n':
			has_n = 1;
			strncpy(name, optarg, MAX_PATH);
			break;
		case 'v':
			has_v = 1;
			strncpy(value, optarg, MAX_VALUE);
			break;
		case 'x':
			has_x = 1;
			strncpy(name, optarg, MAX_PATH);
			break;
		case '?':
		default:
			print_usage();
			errno = EINVAL;
			return -1;
		}
	}

	if (argc < optind + 1 || (!has_n && !has_x) || (has_n && has_x) || (has_x && has_v))
	{
		print_usage();
		errno = EINVAL;
		return -1;
	}

	for (int i = optind; i < argc; ++i)
	{
		if (not_follow)
		{
			if (has_n)
			{
				if (lsetxattr(argv[i], name, value, strlen(value), 0) != 0)
					return -1;
			}
			else if (lremovexattr(argv[i], name) != 0)
				return -1;
		}
		else
		{
			if (has_n)
			{
				if (setxattr(argv[i], name, value, strlen(value), 0) != 0)
					return -1;
			}
			else if (removexattr(argv[i], name) != 0)
				return -1;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("usage: command\n");
		exit(EXIT_SUCCESS);
	}
	if (my_setfattr(argc - 1, &argv[1]) == -1)
		errExit("my_setfattr");

	return 0;
}
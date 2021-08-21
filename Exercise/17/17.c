#include <acl/libacl.h>
#include <sys/acl.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
	int is_user = 0;
	char *name;
	struct passwd *pwd;
	struct group *grp;

	if (argc != 3 || !strcmp(argv[1], "-help"))
	{
		printf("usage: %s <u|g> <user|group> <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (!strcmp(argv[1], "u"))
	{
		is_user = 1;
		if (strspn(argv[2], "0123456789") == strlen(argv[2])) // 全数字
		{
			errno = 0;
			pwd = getpwuid(atoi(argv[2]));
			if (pwd == NULL)
			{
				if (errno == 0) // 找不到说明之前那个就是字符型的名
					name = argv[2];
				else
					errExit("getpwuid");
			}
			else
				name = pwd->pw_name;
		}
		else
			name = argv[2];
	}
	else if (!strcmp(argv[1], "g"))
	{
		if (strspn(argv[2], "0123456789") == strlen(argv[2])) // 全数字
		{
			errno = 0;
			grp = getgrgid(atoi(argv[2]));
			if (grp == NULL)
			{
				if (errno == 0) // 找不到说明之前那个就是字符串形式的名
					name = argv[2];
				else
					errExit("getpwuid");
			}
			else
				name = grp->gr_name;
		}
		else
			name = argv[2];
	}
	else
	{
		printf("usage: %s <u|g> <user|group> <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("%s\n", name);

	return 0;
}
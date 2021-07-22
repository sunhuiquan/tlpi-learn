#include <tlpi_hdr.h>

int myaccess(const char *name, int type)
{
	uid_t effective_uid = geteuid();
	gid_t effective_gid = getegid();
	seteuid(getuid());
	setegid(getgid());
	return access(name, type);
	seteuid(effective_uid);
	setegid(effective_gid);
}

int main(int argc, char *argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0)
	{
		printf("usage: <filename>\n");
		exit(EXIT_SUCCESS);
	}

	if (myaccess(argv[1], F_OK) == 0)
		printf("File exists.\n");
	else
		printf("File doesn't exist.\n");

	if (myaccess(argv[1], R_OK) == 0)
		printf("File can read.\n");
	else
		printf("File can't read.\n");

	if (myaccess(argv[1], W_OK) == 0)
		printf("File can write.\n");
	else
		printf("File can't write.\n");

	if (myaccess(argv[1], X_OK) == 0)
		printf("File can execute.\n");
	else
		printf("File can't execute.\n");

	return 0;
}
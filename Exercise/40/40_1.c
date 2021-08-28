#include <time.h>
#include <lastlog.h>
#include <paths.h> /* Definition of _PATH_LASTLOG */
#include <fcntl.h>
#include <pwd.h>
#include "tlpi_hdr.h"

uid_t userIdFromName(const char *name);

int main(int argc, char *argv[])
{
	struct lastlog llog;
	int fd, j;
	uid_t uid;

	if (argc > 1 && strcmp(argv[1], "--help") == 0)
		usageErr("%s [username...]\n", argv[0]);

	fd = open(_PATH_LASTLOG, O_RDONLY);
	if (fd == -1)
		errExit("open");

	for (j = 1; j < argc; j++)
	{
		uid = userIdFromName(argv[j]);
		if (uid == -1)
		{
			printf("No such user: %s\n", argv[j]);
			continue;
		}

		if (lseek(fd, uid * sizeof(struct lastlog), SEEK_SET) == -1)
			errExit("lseek");

		if (read(fd, &llog, sizeof(struct lastlog)) <= 0)
		{
			printf("read failed for %s\n", argv[j]); /* EOF or error */
			continue;
		}

		time_t ll_time = llog.ll_time;
		printf("%-8.8s %-6.6s %-20.20s %s", argv[j], llog.ll_line,
			   llog.ll_host, ctime((time_t *)&ll_time));
	}

	close(fd);
	exit(EXIT_SUCCESS);
}

uid_t /* Return UID corresponding to 'name', or -1 on error */
userIdFromName(const char *name)
{
	struct passwd *pwd;
	uid_t u;
	char *endptr;

	if (name == NULL || *name == '\0') /* On NULL or empty string */
		return -1;					   /* return an error */

	u = strtol(name, &endptr, 10); /* As a convenience to caller */
	if (*endptr == '\0')		   /* allow a numeric string */
		return u;

	pwd = getpwnam(name);
	if (pwd == NULL)
		return -1;

	return pwd->pw_uid;
}

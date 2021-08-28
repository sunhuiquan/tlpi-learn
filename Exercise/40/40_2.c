#define _GNU_SOURCE
#include <time.h>
#include <utmpx.h>
#include <lastlog.h>
#include <fcntl.h>
#include <paths.h>
#include "../../tlpi-dist/users_groups/ugid_functions.h"
#include "tlpi_hdr.h"

#define MAX_USERNAME 32
#define MAX_USERLINE 32
#define MAX_HOST 256

int update_lastlog(struct utmpx *ut);

int main(int argc, char *argv[])
{
	struct utmpx ut;
	char *devName;

	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s username [sleep-time]\n", argv[0]);

	/* Initialize login record for utmp and wtmp files */

	memset(&ut, 0, sizeof(struct utmpx));
	ut.ut_type = USER_PROCESS; /* This is a user login */
	strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));
	if (time((time_t *)&ut.ut_tv.tv_sec) == -1)
		errExit("time"); /* Stamp with current time */
	ut.ut_pid = getpid();

	/* Set ut_line and ut_id based on the terminal associated with
       'stdin'. This code assumes terminals named "/dev/[pt]t[sy]*".
       The "/dev/" dirname is 5 characters; the "[pt]t[sy]" filename
       prefix is 3 characters (making 8 characters in all). */

	devName = ttyname(STDIN_FILENO);
	if (devName == NULL)
		errExit("ttyname");
	if (strlen(devName) <= 8) /* Should never happen */
		fatal("Terminal name is too short: %s", devName);

	strncpy(ut.ut_line, devName + 5, sizeof(ut.ut_line));
	strncpy(ut.ut_id, devName + 8, sizeof(ut.ut_id));

	printf("Creating login entries in utmp and wtmp\n");
	printf("        using pid %ld, line %.*s, id %.*s\n",
		   (long)ut.ut_pid, (int)sizeof(ut.ut_line), ut.ut_line,
		   (int)sizeof(ut.ut_id), ut.ut_id);

	setutxent();				 /* Rewind to start of utmp file */
	if (pututxline(&ut) == NULL) /* Write login record to utmp */
		errExit("pututxline");
	updwtmpx(_PATH_WTMP, &ut); /* Append login record to wtmp */

	// 这部分是登录所以加在这里
	update_lastlog(&ut);

	/* Sleep a while, so we can examine utmp and wtmp files */

	sleep((argc > 2) ? getInt(argv[2], GN_NONNEG, "sleep-time") : 15);

	/* Now do a "logout"; use values from previously initialized 'ut',
       except for changes below */

	ut.ut_type = DEAD_PROCESS;		  /* Required for logout record */
	time((time_t *)&ut.ut_tv.tv_sec); /* Stamp with logout time */
	memset(&ut.ut_user, 0, sizeof(ut.ut_user));
	/* Logout record has null username */

	printf("Creating logout entries in utmp and wtmp\n");
	setutxent();				 /* Rewind to start of utmp file */
	if (pututxline(&ut) == NULL) /* Overwrite previous utmp record */
		errExit("pututxline");
	updwtmpx(_PATH_WTMP, &ut); /* Append logout record to wtmp */

	endutxent();
	exit(EXIT_SUCCESS);
}

int update_lastlog(struct utmpx *ut)
{
	char username[MAX_USERNAME];
	struct lastlog llog;
	int fd;
	uid_t uid;

	fd = open(_PATH_LASTLOG, O_RDONLY);
	if (fd == -1)
		return -1;

	strncpy(username, ut->ut_user, MAX_USERNAME);
	uid = userIdFromName(username);
	if (uid == -1)
		return -1;

	if (lseek(fd, uid * sizeof(struct lastlog), SEEK_SET) == -1)
		return -1;

	strncpy(llog.ll_host, ut->ut_host, MAX_HOST);
	strncpy(llog.ll_line, ut->ut_line, MAX_USERLINE);
	llog.ll_time = time(NULL); // 获取登录的此时时间

	if (write(fd, &llog, sizeof(struct lastlog)) != sizeof(struct lastlog))
		return -1;

	close(fd);
	return 0;
}

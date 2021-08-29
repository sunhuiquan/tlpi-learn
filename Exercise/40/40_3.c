#define _GNU_SOURCE // updwtmpx is included

#include <unistd.h>
#include <stdlib.h>
#include <utmpx.h>
#include <paths.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <tlpi_hdr.h>

static void my_logwtmp(const char *line, const char *name, const char *host);

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("%s usage: <line> <name> <host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	my_logwtmp(argv[1], argv[2], argv[3]);

	exit(EXIT_SUCCESS);
}

static void
my_logwtmp(const char *line, const char *name, const char *host)
{
	struct utmpx ut;

	strncpy(ut.ut_line, line, __UT_LINESIZE);
	strncpy(ut.ut_user, name, __UT_NAMESIZE);
	strncpy(ut.ut_host, host, __UT_HOSTSIZE);
	time((time_t *)&ut.ut_tv.tv_sec);
	ut.ut_pid = getpid();
	ut.ut_type = strlen(name) == 0 ? DEAD_PROCESS : USER_PROCESS;

	updwtmpx(_PATH_WTMP, &ut);
}
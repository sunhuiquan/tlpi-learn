#include <unistd.h>
#include <utmpx.h>
#include <utmp.h>
#include <errno.h>
#include <paths.h>
#include <time.h>
#include <stdio.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
	struct utmpx *ut;
	char time[32];

	if (argc > 2)
	{
		printf("%s usage: [path]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (argc > 1)
		if (utmpname(argv[1]) == -1)
			errExit("utmpname");

	setutxent();

	while ((ut = getutxent()) != NULL)
	{
		if (ut->ut_type == INIT_PROCESS || ut->ut_type == LOGIN_PROCESS || ut->ut_type == USER_PROCESS)
		{
			struct tm *t = localtime((time_t *)&ut->ut_tv.tv_sec);
			strftime(time, 32, "%F %T", t);
			printf("%10s %10s %20s\n", ut->ut_user, ut->ut_line, time);
		}
	}

	endutxent();
	exit(EXIT_SUCCESS);
}
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <tlpi_hdr.h>

#define MAXIDENTY 1024

int main(int argc, char *argv[])
{
	int has_identy = 0;
	int opt = 0, facility, level;
	char identy[MAXIDENTY];

	while ((opt = getopt(argc, argv, "hi:f:l:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			printf("%s usage: [-i ident] [-f facility] [-l level] <msg>", argv[0]);
			exit(EXIT_SUCCESS);
			break;

		case 'i':
			strncpy(identy, optarg, MAXIDENTY);
			has_identy = 1;
			break;

		case 'f':
			if (!strcmp(optarg, "auth"))
				facility = LOG_AUTH;
			else if (!strcmp(optarg, "authpriv"))
				facility = LOG_AUTHPRIV;
			else if (!strcmp(optarg, "cron"))
				facility = LOG_CRON;
			else if (!strcmp(optarg, "daemon"))
				facility = LOG_DAEMON;
			else if (!strcmp(optarg, "ftp"))
				facility = LOG_FTP;
			else if (!strcmp(optarg, "lpr"))
				facility = LOG_LPR;
			else if (!strcmp(optarg, "mail"))
				facility = LOG_MAIL;
			else if (!strcmp(optarg, "news"))
				facility = LOG_NEWS;
			else if (!strcmp(optarg, "syslog"))
				facility = LOG_SYSLOG;
			else if (!strcmp(optarg, "user"))
				facility = LOG_USER;
			else if (!strcmp(optarg, "uucp"))
				facility = LOG_UUCP;
			break;

		case 'l':
			if (!strcmp(optarg, "emerg"))
				level = LOG_EMERG;
			else if (!strcmp(optarg, "alert"))
				level = LOG_ALERT;
			else if (!strcmp(optarg, "crit"))
				level = LOG_CRIT;
			else if (!strcmp(optarg, "err"))
				level = LOG_ERR;
			else if (!strcmp(optarg, "warning"))
				level = LOG_WARNING;
			else if (!strcmp(optarg, "notice"))
				level = LOG_NOTICE;
			else if (!strcmp(optarg, "info"))
				level = LOG_INFO;
			else if (!strcmp(optarg, "debug"))
				level = LOG_DEBUG;
			break;

		case '?':
		default:
			printf("%s usage: [-i ident] [-f facility] [-l level] <msg>", argv[0]);
			exit(EXIT_FAILURE);
		}

		if (has_identy)
			openlog(identy, LOG_PID, facility);
		else
			openlog(NULL, LOG_PID, facility);
		// syslog(level, "%s", msg);
	}

	if (optind == argc - 1)
	{
	}

	return 0;
}

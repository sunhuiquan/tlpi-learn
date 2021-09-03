#define _GNU_SOURCE /* Get strsignal() declaration from <string.h> */
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void /* Handler for SIGHUP */
handler(int sig)
{
	printf("PID %ld: caught signal %2d (%s)\n", (long)getpid(),
		   sig, strsignal(sig));
	/* UNSAFE (see Section 21.1.2) */
}

static void /* Handler for SIGHUP */
handler_main(int sig)
{
	printf("PID %ld: main caught signal %2d (%s)\n", (long)getpid(),
		   sig, strsignal(sig));
	/* UNSAFE (see Section 21.1.2) */
}

int main(int argc, char *argv[])
{
	pid_t parentPid, childPid;
	int j;
	struct sigaction sa;

	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s {d|s}... [ > sig.log 2>&1 ]\n", argv[0]);

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler_main;
	if (sigaction(SIGHUP, &sa, NULL) == -1)
		errExit("sigaction");

	setbuf(stdout, NULL); /* Make stdout unbuffered */

	parentPid = getpid();
	printf("PID of parent process is:       %ld\n", (long)parentPid);
	printf("Foreground process group ID is: %ld\n",
		   (long)tcgetpgrp(STDIN_FILENO));

	for (j = 1; j < argc; j++)
	{ /* Create child processes */
		childPid = fork();
		if (childPid == -1)
			errExit("fork");

		if (childPid == 0)
		{						   /* If child... */
			if (argv[j][0] == 'd') /* 'd' --> to different pgrp */
				if (setpgid(0, 0) == -1)
					errExit("setpgid");

			sigemptyset(&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = handler;
			if (sigaction(SIGHUP, &sa, NULL) == -1)
				errExit("sigaction");
			break; /* Child exits loop */
		}
	}

	/* All processes fall through to here */

	alarm(60); /* Ensure each process eventually terminates */

	printf("PID=%ld PGID=%ld\n", (long)getpid(), (long)getpgrp());
	for (;;)
		pause(); /* Wait for signals */
}

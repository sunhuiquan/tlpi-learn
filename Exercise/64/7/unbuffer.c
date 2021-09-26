#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <tlpi_hdr.h>
#include <time.h>
#include <signal.h>
#include "../../../tlpi-dist/pty/pty_fork.h"	  /* Declaration of ptyFork() */
#include "../../../tlpi-dist/tty/tty_functions.h" /* Declaration of ttySetRaw() */

#define BUF_SIZE 256
#define MAX_SNAME 1000
#define MAXTIMELEN 128

struct termios ttyOrig;
int masterFd;

void sigwinch_handler(int sig)
{
	int saved_errno = errno;

	struct winsize ws;
	// if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1)
	// 	errExit("ioctl");
	// if (ioctl(masterFd, TIOCSWINSZ, &ws) == -1)
	// 	errExit("ioctl");

	// 因为终端大小设置失败也不致命，只要保护好errno值就行
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) != -1)
		ioctl(masterFd, TIOCSWINSZ, &ws);

	errno = saved_errno;
}

static void /* Reset terminal mode on program exit */
ttyReset(void)
{
	if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1)
		errExit("tcsetattr");
}

int main(int argc, char *argv[])
{
	char slaveName[MAX_SNAME];
	char *shell;
	struct winsize ws;
	fd_set inFds;
	char buf[BUF_SIZE];
	ssize_t numRead;
	pid_t childPid;
	time_t curr_time;
	char *pctime;
	char time_str[MAXTIMELEN];
	struct sigaction act;

	/* Retrieve the attributes of terminal on which we are started */

	if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
		errExit("tcgetattr");
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
		errExit("ioctl-TIOCGWINSZ");

	/* Create a child process, with parent and child connected via a
       pty pair. The child is connected to the pty slave and its terminal
       attributes are set to be the same as those retrieved above. */

	childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws);
	if (childPid == -1)
		errExit("ptyFork");

	if (childPid == 0)
	{ /* Child: execute a shell on pty slave */
		if (time(&curr_time) == -1)
			errExit("time");
		pctime = ctime(&curr_time);
		printf("Script started on %s\n", (pctime == NULL) ? "can't get time" : pctime);

		/* If the SHELL variable is set, use its value to determine
           the shell execed in child. Otherwise use /bin/sh. */

		shell = getenv("SHELL");
		if (shell == NULL || *shell == '\0')
			shell = "/bin/sh";

		execlp(shell, shell, (char *)NULL);
		errExit("execlp"); /* If we get here, something went wrong */
	}

	/* Parent: relay data between terminal and pty master */

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	act.sa_handler = sigwinch_handler;
	if (sigaction(SIGWINCH, &act, NULL) == -1)
		errExit("sigaction");

	/* Place terminal in raw mode so that we can pass all terminal
     input to the pseudoterminal master untouched */

	ttySetRaw(STDIN_FILENO, &ttyOrig);

	if (atexit(ttyReset) != 0)
		errExit("atexit");

	if (write(masterFd, argv[1], strlen(argv[1])) != strlen(argv[1]))
		fatal("partial/failed write (masterFd)");
	for (;;)
	{
		numRead = read(masterFd, buf, BUF_SIZE);
		if (numRead <= 0) // 正常退出也是通过这个途径
		{
			if (time(&curr_time) == -1)
				errExit("time");
			pctime = ctime(&curr_time);
			snprintf(time_str, MAXTIMELEN, "Script started on %s\n", (pctime == NULL) ? "can't get time" : pctime);
			printf("%s\n", time_str);
			exit(EXIT_SUCCESS);
		}

		if (write(STDOUT_FILENO, buf, numRead) != numRead)
			fatal("partial/failed write (STDOUT_FILENO)");
	}
}

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

static void /* Reset terminal mode on program exit */
ttyReset(void)
{
	if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1)
		errExit("tcsetattr");
}

int main(int argc, char *argv[])
{
	char slaveName[MAX_SNAME];
	int masterFd;
	struct winsize ws;
	fd_set inFds;
	char buf[BUF_SIZE];
	ssize_t numRead;
	pid_t childPid;
	time_t curr_time;
	char *pctime;

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
	{
		// child
		execlp("login", "login", "to do", (char *)NULL);
		errExit("execlp"); /* If we get here, something went wrong */
	}

	// parent
	ttySetRaw(STDIN_FILENO, &ttyOrig);

	if (atexit(ttyReset) != 0)
		errExit("atexit");

	for (;;)
	{
		FD_ZERO(&inFds);
		FD_SET(STDIN_FILENO, &inFds);
		FD_SET(masterFd, &inFds);

		if (select(masterFd + 1, &inFds, NULL, NULL, NULL) == -1)
		{
			if (errno = EINTR)
				continue;
			else
				errExit("select");
		}

		if (FD_ISSET(STDIN_FILENO, &inFds))
		{ /* stdin --> pty */
			numRead = read(STDIN_FILENO, buf, BUF_SIZE);
			if (numRead <= 0) // 正常退出^D也是通过这个途径
				exit(EXIT_SUCCESS);

			if (write(masterFd, buf, numRead) != numRead)
				fatal("partial/failed write (masterFd)");
		}
		if (FD_ISSET(masterFd, &inFds))
		{ /* pty --> stdout+file */
			numRead = read(masterFd, buf, BUF_SIZE);
			if (numRead <= 0) // 正常退出也是通过这个途径
				exit(EXIT_SUCCESS);

			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				fatal("partial/failed write (STDOUT_FILENO)");
		}
	}
}

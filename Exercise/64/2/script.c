#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/select.h>
#include <tlpi_hdr.h>
#include <time.h>
#include "../../../tlpi-dist/pty/pty_fork.h"	  /* Declaration of ptyFork() */
#include "../../../tlpi-dist/tty/tty_functions.h" /* Declaration of ttySetRaw() */

#define BUF_SIZE 256
#define MAX_SNAME 1000

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
	char *shell;
	int masterFd, scriptFd;
	struct winsize ws;
	fd_set inFds;
	char buf[BUF_SIZE];
	ssize_t numRead;
	pid_t childPid;
	time_t start_time, end_time;

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
		// if(time())

		/* If the SHELL variable is set, use its value to determine
           the shell execed in child. Otherwise use /bin/sh. */

		shell = getenv("SHELL");
		if (shell == NULL || *shell == '\0')
			shell = "/bin/sh";

		execlp(shell, shell, (char *)NULL);
		errExit("execlp"); /* If we get here, something went wrong */
	}

	/* Parent: relay data between terminal and pty master */

	scriptFd = open((argc > 1) ? argv[1] : "typescript",
					O_WRONLY | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
						S_IROTH | S_IWOTH);
	if (scriptFd == -1)
		errExit("open typescript");

	/* Place terminal in raw mode so that we can pass all terminal
     input to the pseudoterminal master untouched */

	ttySetRaw(STDIN_FILENO, &ttyOrig);

	if (atexit(ttyReset) != 0)
		errExit("atexit");

	/* Loop monitoring terminal and pty master for input. If the
       terminal is ready for input, then read some bytes and write
       them to the pty master. If the pty master is ready for input,
       then read some bytes and write them to the terminal. */

	for (;;)
	{
		FD_ZERO(&inFds);
		FD_SET(STDIN_FILENO, &inFds);
		FD_SET(masterFd, &inFds);

		if (select(masterFd + 1, &inFds, NULL, NULL, NULL) == -1)
			errExit("select");

		if (FD_ISSET(STDIN_FILENO, &inFds))
		{ /* stdin --> pty */
			numRead = read(STDIN_FILENO, buf, BUF_SIZE);
			if (numRead <= 0)
				exit(EXIT_SUCCESS);

			if (write(masterFd, buf, numRead) != numRead)
				fatal("partial/failed write (masterFd)");
		}

		if (FD_ISSET(masterFd, &inFds))
		{ /* pty --> stdout+file */
			numRead = read(masterFd, buf, BUF_SIZE);
			if (numRead <= 0)
				exit(EXIT_SUCCESS);

			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				fatal("partial/failed write (STDOUT_FILENO)");
			if (write(scriptFd, buf, numRead) != numRead)
				fatal("partial/failed write (scriptFd)");
		}
	}
}

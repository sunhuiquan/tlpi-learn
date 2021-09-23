#define _GNU_SOURCE

#include <signal.h>
#include <ctype.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <tlpi_hdr.h>
#include "../../../tlpi-dist/tty/tty_functions.h" /* Declaration of ttySetCbreak() */

static volatile sig_atomic_t gotSigio = 0;
/* Set nonzero on receipt of SIGIO */

static void
sigrtminHandler(int sig, siginfo_t *si, void *context)
{
	// unsafe here
	printf("signo: %d, si_fd: %d, si_code: %s\n", si->si_signo, si->si_fd,
		   (si->si_code == SI_USER) ? "SI_USER" : (si->si_code == SI_QUEUE) ? "SI_QUEUE"
																			: "other");
	gotSigio = 1;
}

int main(int argc, char *argv[])
{
	int flags, j, cnt;
	struct termios origTermios;
	char ch;
	struct sigaction sa;
	Boolean done;

	/* Establish handler for "I/O possible" signal */

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	sa.sa_sigaction = sigrtminHandler;
	if (sigaction(SIGRTMIN, &sa, NULL) == -1)
		errExit("sigaction");

	/* Set owner process that is to receive "I/O possible" signal */

	if (fcntl(STDIN_FILENO, F_SETOWN, getpid()) == -1)
		errExit("fcntl(F_SETOWN)");

	/* Enable "I/O possible" signaling and make I/O nonblocking
       for file descriptor */
	if (fcntl(STDIN_FILENO, F_SETSIG, SIGRTMIN) == -1)
		errExit("fcntl(F_SETSIG)");

	flags = fcntl(STDIN_FILENO, F_GETFL);
	if (fcntl(STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1)
		errExit("fcntl(F_SETFL)");

	/* Place terminal in cbreak mode */

	if (ttySetCbreak(STDIN_FILENO, &origTermios) == -1)
		errExit("ttySetCbreak");

	for (done = FALSE, cnt = 0; !done; cnt++)
	{
		for (j = 0; j < 100000000; j++)
			continue; /* Slow main loop down a little */

		if (gotSigio)
		{ /* Is input available? */
			gotSigio = 0;

			/* Read all available input until error (probably EAGAIN)
               or EOF (not actually possible in cbreak mode) or a
               hash (#) character is read */

			while (read(STDIN_FILENO, &ch, 1) > 0 && !done)
			{
				printf("cnt=%d; read %c\n", cnt, ch);
				done = ch == '#';
			}
		}
	}

	/* Restore original terminal settings */

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1)
		errExit("tcsetattr");
	exit(EXIT_SUCCESS);
}

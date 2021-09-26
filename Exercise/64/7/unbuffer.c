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

#define MAXLINE 1024

int main(int argc, char *argv[])
{
	char slaveName[MAX_SNAME];
	struct winsize ws;
	char buf[BUF_SIZE];
	ssize_t numRead;
	pid_t childPid;
	struct sigaction act;
	int index, j;
	char args1[argc][MAXLINE], args2[argc][MAXLINE];

	if (argc < 4)
	{
		printf("%s usage: <command1> : <command2>\n", argv[0]);
		printf("注意要 : 因为 | 就是被shell解释成管道，而不是我们的实现\n");
		exit(EXIT_SUCCESS);
	}

	index = 0;
	for (j = 0; j < argc; ++j)
		if (!strcmp(argv[j], ":"))
		{
			index = j;
			break;
		}
	if (j == argc)
		errExit("no : character");

	for (int i = 1, a = 0; i < index; ++i, ++a)
		strncpy(args1[a], argv[i], MAXLINE);
	for (int i = index + 1, a = 0; i < argc; ++i, ++a)
		strncpy(args2[a], argv[i], MAXLINE);

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
		// 已经重定向到从设备
		printf("%ld\n", (long)getpid());
		execvp(args2[0], args2); // p means also search in path dirctionary
		errExit("execlp");		 /* If we get here, something went wrong */
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

	switch (fork())
	{
	case -1:
		errExit("fork");
		break;

	case 0:
		if (dup2(masterFd, STDOUT_FILENO) == -1)
			errExit("dup2");
		if (masterFd != STDOUT_FILENO)
			close(masterFd);

		execvp(args1[0], args1);
		errExit("execlp"); /* If we get here, something went wrong */
		_exit(EXIT_SUCCESS);
		break;

	default:
		break;
	}

	for (;;)
	{
		numRead = read(masterFd, buf, BUF_SIZE);
		if (numRead <= 0) // 正常退出也是通过这个途径
			exit(EXIT_SUCCESS);

		if (write(STDOUT_FILENO, buf, numRead) != numRead)
			fatal("partial/failed write (STDOUT_FILENO)");
	}
}

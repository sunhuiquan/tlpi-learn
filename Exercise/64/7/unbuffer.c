#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <tlpi_hdr.h>
#include <time.h>
#include <signal.h>
#include "../../../tlpi-dist/pty/pty_master_open.h"
#include "../../../tlpi-dist/tty/tty_functions.h" /* Declaration of ttySetRaw() */

int set_non_echo(int fd)
{
	struct termios term;
	if (tcgetattr(fd, &term) == -1)
		return -1;
	term.c_lflag &= ~ECHO;
	term.c_lflag |= ECHONL;
	if (tcsetattr(fd, TCSAFLUSH, &term) == -1)
		return -1;

	return 0;
}

#define MAX_SNAME 1000 /* Maximum size for pty slave name */

pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen,
			  const struct termios *slaveTermios, const struct winsize *slaveWS)
{
	int mfd, slaveFd, savedErrno;
	pid_t childPid;
	char slname[MAX_SNAME];

	mfd = ptyMasterOpen(slname, MAX_SNAME);
	if (mfd == -1)
		return -1;

	if (slaveName != NULL)
	{ /* Return slave name to caller */
		if (strlen(slname) < snLen)
		{
			strncpy(slaveName, slname, snLen);
		}
		else
		{ /* 'slaveName' was too small */
			close(mfd);
			errno = EOVERFLOW;
			return -1;
		}
	}

	childPid = fork();

	if (childPid == -1)
	{						/* fork() failed */
		savedErrno = errno; /* close() might change 'errno' */
		close(mfd);			/* Don't leak file descriptors */
		errno = savedErrno;
		return -1;
	}

	if (childPid != 0)
	{					 /* Parent */
		*masterFd = mfd; /* Only parent gets master fd */
		return childPid; /* Like parent of fork() */
	}

	/* Child falls through to here */

	if (setsid() == -1) /* Start a new session */
		err_exit("ptyFork:setsid");

	close(mfd); /* Not needed in child */

	slaveFd = open(slname, O_RDWR); /* Becomes controlling tty */
	if (slaveFd == -1)
		err_exit("ptyFork:open-slave");

#ifdef TIOCSCTTY /* Acquire controlling tty on BSD */
	if (ioctl(slaveFd, TIOCSCTTY, 0) == -1)
		err_exit("ptyFork:ioctl-TIOCSCTTY");
#endif

	if (slaveTermios != NULL) /* Set slave tty attributes */
		if (tcsetattr(slaveFd, TCSANOW, slaveTermios) == -1)
			err_exit("ptyFork:tcsetattr");

	if (slaveWS != NULL) /* Set slave tty window size */
		if (ioctl(slaveFd, TIOCSWINSZ, slaveWS) == -1)
			err_exit("ptyFork:ioctl-TIOCSWINSZ");

	if (set_non_echo(slaveFd) == -1)
		errExit("set_non_echo");

	/* Duplicate pty slave to be child's stdin, stdout, and stderr */

	if (dup2(slaveFd, STDIN_FILENO) != STDIN_FILENO)
		err_exit("ptyFork:dup2-STDIN_FILENO");
	if (dup2(slaveFd, STDOUT_FILENO) != STDOUT_FILENO)
		err_exit("ptyFork:dup2-STDOUT_FILENO");
	if (dup2(slaveFd, STDERR_FILENO) != STDERR_FILENO)
		err_exit("ptyFork:dup2-STDERR_FILENO");

	if (slaveFd > STDERR_FILENO) /* Safety check */
		close(slaveFd);			 /* No longer need this fd */

	return 0; /* Like child of fork() */
}

#define BUF_SIZE 256
#define MAX_SNAME 1000
#define MAXTIMELEN 128

struct termios ttyOrig;
int masterFd;

void sigchld_handler(int sig)
{
	if (write(masterFd, "\004", 1) != 1)
		errExit("write");
}

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
	char **args1, **args2;

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

	argv[index] = NULL;
	args1 = &argv[1];
	args2 = &argv[index + 1];

	// for (int i = 0; args1[i]; ++i)
	// 	printf("%s\n", args1[i]);
	// for (int i = 0; args2[i]; ++i)
	// 	printf("%s\n", args2[i]);

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
		// printf("%ld\n", (long)getpid());
		execvp(args2[0], args2); // p means also search in path dirctionary
		errExit("execlp");		 /* If we get here, something went wrong */
	}

	/* Parent: relay data between terminal and pty master */

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	act.sa_handler = sigwinch_handler;
	if (sigaction(SIGWINCH, &act, NULL) == -1)
		errExit("sigaction");

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	act.sa_handler = sigchld_handler;
	if (sigaction(SIGCHLD, &act, NULL) == -1)
		errExit("sigaction");

	/* Place terminal in raw mode so that we can pass all terminal
     input to the pseudoterminal master untouched */

	ttySetRaw(STDIN_FILENO, &ttyOrig);

	if (set_non_echo(masterFd) == -1)
		errExit("set_disp_mode");

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

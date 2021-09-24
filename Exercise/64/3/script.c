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

void sigchld_hander(int sig)
{
	// 子进程关闭，通知父进程也该关了
	// printf("parent exit\n"); // unsafe only ues it for testing
	exit(EXIT_SUCCESS);
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
	int scriptFd;
	struct winsize ws;
	char buf[BUF_SIZE];
	ssize_t numRead;
	pid_t childPid;
	time_t curr_time;
	char *pctime;
	char time_str[MAXTIMELEN];
	struct sigaction act;
	pid_t pid;

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

	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	// 一定要用阻塞，避免在无限循环里面疯狂自旋，浪费CPU
	case 0:
		// childL: this process is for masterFD
		for (;;)
		{
			numRead = read(masterFd, buf, BUF_SIZE);
			if (numRead <= 0) // 正常退出也是通过这个途径
			{
				if (time(&curr_time) == -1)
					errExit("time");
				pctime = ctime(&curr_time);
				snprintf(time_str, MAXTIMELEN, "Script ended on %s\n", (pctime == NULL) ? "can't get time" : pctime);
				printf("%s", time_str);
				if (write(scriptFd, time_str, strlen(time_str)) != strlen(time_str))
					fatal("partial/failed write (scriptFd)");
				// printf("child exit\n");
				_exit(EXIT_SUCCESS);
			}

			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				fatal("partial/failed write (STDOUT_FILENO)");
			if (write(scriptFd, buf, numRead) != numRead)
				fatal("partial/failed write (scriptFd)");
		}
		_exit(EXIT_SUCCESS);
		break;

	default:
		// parent: this process is for STDIN_FILENO
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		act.sa_handler = sigchld_hander;
		if (sigaction(SIGCHLD, &act, NULL) == -1)
			errExit("sigaction");

		// 不用担心子shell在sigaction前终止，因为向主设备写的操作是在sigaction之后的，所以安全
		// (详细过程:从终端向主设备写，之后主设备到从设备，子shell再从从设备读)
		for (;;)
		{
			numRead = read(STDIN_FILENO, buf, BUF_SIZE);
			if (numRead <= 0)
				errExit("read");

			if (write(masterFd, buf, numRead) != numRead)
				fatal("partial/failed write (masterFd)");
		}
		break;
	}
	exit(EXIT_SUCCESS);
}

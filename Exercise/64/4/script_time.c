#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <tlpi_hdr.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include "../../../tlpi-dist/pty/pty_fork.h"	  /* Declaration of ptyFork() */
#include "../../../tlpi-dist/tty/tty_functions.h" /* Declaration of ttySetRaw() */

#define BUF_SIZE 256
#define MAX_SNAME 1000
#define MAXTIMELEN 128

struct termios ttyOrig;
int masterFd;

int passtime(const struct timeval *start);

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
	int scriptFd, replayFd;
	struct winsize ws;
	fd_set inFds;
	char buf[BUF_SIZE];
	char buf2[BUF_SIZE + 20];
	char command[BUF_SIZE];
	ssize_t numRead;
	pid_t childPid;
	time_t curr_time;
	char *pctime;
	char time_str[MAXTIMELEN];
	struct sigaction act;
	struct timeval start_tv;
	int pass_time;
	int index;

	/* Retrieve the attributes of terminal on which we are started */

	if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
		errExit("tcgetattr");
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
		errExit("ioctl-TIOCGWINSZ");

	/* Create a child process, with parent and child connected via a
       pty pair. The child is connected to the pty slave and its terminal
       attributes are set to be the same as those retrieved above. */

	if (gettimeofday(&start_tv, NULL) == -1)
		errExit("gettimeofday");

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

	replayFd = open("replay", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (replayFd == -1)
		errExit("open replay");

	/* Place terminal in raw mode so that we can pass all terminal
     input to the pseudoterminal master untouched */

	ttySetRaw(STDIN_FILENO, &ttyOrig);

	if (atexit(ttyReset) != 0)
		errExit("atexit");

	/* Loop monitoring terminal and pty master for input. If the
       terminal is ready for input, then read some bytes and write
       them to the pty master. If the pty master is ready for input,
       then read some bytes and write them to the terminal. */

	index = 0;
	command[0] = '\0';
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

			// buf[numRead] = '\0';
			// printf("[buf: %s]\n", buf);

			for (int i = 0; i < numRead; ++i)
			{
				if (buf[i] == 4)
					break; // ^D
				if (index == BUF_SIZE)
					errExit("overflow");
				if (buf[i] == 127) // delete 控制字符
				{
					if (index > 0)
						--index;
				}
				else
					command[index++] = buf[i];

				if (buf[i] == '\r') // 注意这是原始模式的终端
				{
					command[index - 1] = '\n';
					if (index == BUF_SIZE)
						errExit("overflow");
					command[index] = '\0';
					if ((pass_time = passtime(&start_tv)) == -1)
						errExit("passtime");
					sprintf(buf2, "%d %s", pass_time, command); // 这里是%s要注意'\0'必须要有不然溢出
					printf("command: %s\n", command);
					if (write(replayFd, buf2, strlen(buf2)) != strlen(buf2))
						fatal("partial/failed write (scriptFd)");
					index = 0;
					command[index] = '\0';
				}
			}

			if (write(masterFd, buf, numRead) != numRead)
				fatal("partial/failed write (masterFd)");
		}
		if (FD_ISSET(masterFd, &inFds))
		{ /* pty --> stdout+file */
			numRead = read(masterFd, buf, BUF_SIZE);
			if (numRead <= 0) // 正常退出也是通过这个途径
			{
				if (time(&curr_time) == -1)
					errExit("time");
				pctime = ctime(&curr_time);
				snprintf(time_str, MAXTIMELEN, "Script started on %s\n", (pctime == NULL) ? "can't get time" : pctime);
				printf("%s\n", time_str);
				if (write(scriptFd, time_str, strlen(time_str)) != strlen(time_str))
					fatal("partial/failed write (scriptFd)");
				exit(EXIT_SUCCESS);
			}

			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				fatal("partial/failed write (STDOUT_FILENO)");
			if (write(scriptFd, buf, numRead) != numRead)
				fatal("partial/failed write (scriptFd)");
		}
	}
}

int passtime(const struct timeval *start)
{
	struct timeval tv;
	if (!start || gettimeofday(&tv, NULL) == -1)
		return -1;
	return (tv.tv_sec - start->tv_sec) * 1000000 + (tv.tv_usec - start->tv_usec);
}
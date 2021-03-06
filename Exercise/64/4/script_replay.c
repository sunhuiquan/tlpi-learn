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

int readline(int fd, char *buf, int sz);

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
	char buf[BUF_SIZE], buf2[BUF_SIZE];
	ssize_t numRead;
	pid_t childPid;
	time_t curr_time;
	char *pctime;
	struct sigaction act;
	int maxfd;
	long cts;
	struct timeval start, curr;
	int index;
	int pfd[2];
	char *pbuf, command[BUF_SIZE];
	int readn, closed_replay_select = 0;
	char ch;

	/* Retrieve the attributes of terminal on which we are started */

	if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
		errExit("tcgetattr");
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
		errExit("ioctl-TIOCGWINSZ");

	/* Create a child process, with parent and child connected via a
       pty pair. The child is connected to the pty slave and its terminal
       attributes are set to be the same as those retrieved above. */

	if (gettimeofday(&start, NULL) == -1)
		errExit("gettimeofday");

	childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws);
	if (childPid == -1)
		errExit("ptyFork");

	if (childPid == 0)
	{ /* Child: execute a shell on pty slave */
		if (time(&curr_time) == -1)
			errExit("time");
		pctime = ctime(&curr_time);
		printf("Use 'q' to quit and script started on %s\n", (pctime == NULL) ? "can't get time" : pctime);

		/* If the SHELL variable is set, use its value to determine
           the shell execed in child. Otherwise use /bin/sh. */

		shell = getenv("SHELL");
		if (shell == NULL || *shell == '\0')
			shell = "/bin/sh";

		execlp(shell, shell, (char *)NULL);
		errExit("execlp"); /* If we get here, something went wrong */
	}

	/* Parent: relay data between terminal and pty master */

	if (pipe(pfd) == -1)
		errExit("pipe");

	command[0] = '\0';
	switch (fork())
	{
	case -1:
		errExit("fork");
		break;

	case 0:
		close(pfd[1]);
		for (;;)
		{
			readn = readline(pfd[0], command, BUF_SIZE);
			if (readn == 0) // 无数据可读，父进程关闭后会返回EOF
				_exit(EXIT_SUCCESS);
			else if (readn < 0)
				errExit("readline");

			command[readn] = '\0'; // 记住strlen和read的大小含转义字符，除了'\0'
			if ((pbuf = strchr(command, ' ')) == NULL)
				errExit("format wrong");
			*pbuf = '\0';
			++pbuf;
			if (sscanf(command, "%ld", &cts) != 1)
				errExit("sscanf");

			// printf("%ld - %s", cts, pbuf);

			// 这里为了简化测试我们用了自旋，推荐定时器加信号的方式，避免自旋浪费CPU
			do
			{
				if (gettimeofday(&curr, NULL) == -1)
					errExit("gettimeofday");
			} while (((curr.tv_sec - start.tv_sec) * 1000000 + (curr.tv_usec - curr.tv_usec)) < cts);

			// printf("%ld - %ld\n", cts, (curr.tv_sec - start.tv_sec) * 1000000 + (curr.tv_usec - curr.tv_usec));
			// printf("command: %s", pbuf);
			if (write(masterFd, pbuf, strlen(pbuf)) != strlen(pbuf))
				fatal("partial/failed write (masterFd)");
			command[0] = '\0';
		}
		_exit(EXIT_FAILURE);
		break;

	default:
		close(pfd[0]);
		break;
	}

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	act.sa_handler = sigwinch_handler;
	if (sigaction(SIGWINCH, &act, NULL) == -1)
		errExit("sigaction");

	scriptFd = open((argc > 1) ? argv[1] : "typescript_replay",
					O_WRONLY | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
						S_IROTH | S_IWOTH);
	if (scriptFd == -1)
		errExit("open typescript");

	replayFd = open((argc > 1) ? argv[1] : "replay", O_RDONLY);
	if (replayFd == -1)
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

	index = 0;
	maxfd = max(masterFd, replayFd);
	for (;;)
	{
		FD_ZERO(&inFds);
		if (closed_replay_select == 0)
			FD_SET(replayFd, &inFds);
		FD_SET(masterFd, &inFds);
		FD_SET(STDIN_FILENO, &inFds);

		if (select(maxfd + 1, &inFds, NULL, NULL, NULL) == -1)
		{
			if (errno = EINTR)
				continue;
			else
				errExit("select");
		}

		if (FD_ISSET(replayFd, &inFds))
		{ /* stdin --> pty */
			numRead = read(replayFd, buf, BUF_SIZE);
			if (numRead < 0) // 正常退出^D也是通过这个途径
				errExit("read");
			else if (numRead == 0)
			{ // 读到文件结束会读到一个EOF告知
				close(pfd[1]);
				closed_replay_select = 1;
				FD_CLR(replayFd, &inFds);
			}
			else
			{
				for (int i = 0; i < numRead; ++i)
				{
					buf2[index++] = buf[i];
					if (buf[i] == '\n')
					{
						buf2[index] = '\0'; // 这样strlen就能计算了
						if (write(pfd[1], buf2, strlen(buf2)) != strlen(buf2))
							errExit("write");
						index = 0;
					}
				}
			}
		}

		if (FD_ISSET(masterFd, &inFds))
		{ /* pty --> stdout+file */
			numRead = read(masterFd, buf, BUF_SIZE);
			if (numRead <= 0) // 因为这个重放不能通过EOF退出
				errExit("read");

			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				fatal("partial/failed write (STDOUT_FILENO)");
			if (write(scriptFd, buf, numRead) != numRead)
				fatal("partial/failed write (scriptFd)");
		}

		if (FD_ISSET(STDIN_FILENO, &inFds))
		{
			numRead = read(STDIN_FILENO, &ch, 1);
			if (numRead < 0)
				errExit("read");
			if (ch == 'q')
				exit(EXIT_SUCCESS);
		}
	}
}

int readline(int fd, char *buf, int sz)
{
	char buffer[BUF_SIZE], ch; // 静态变量初始值就是0，只有局部变量才要初始化
	int index, readn;

	index = 0;
	while (true)
	{
		if ((readn = read(fd, &ch, 1)) <= 0)
			return readn;

		buffer[index++] = ch;
		if (ch == '\n')
		{
			buffer[index] = '\0';
			strncpy(buf, buffer, BUF_SIZE); // 这个是把'\0'也复制过去
			return index;
		}
	}
}
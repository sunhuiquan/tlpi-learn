#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <errno.h>
#include <tlpi_hdr.h>

int my_nice(pid_t pid, int incr);

int main(int argc, char *argv[])
{
	int incr, nice;
	pid_t pid;

	if (argc != 3)
	{
		printf("%s usage: <pid> <incr>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pid = (pid_t)atoi(argv[1]);
	incr = atoi(argv[2]);

	errno = 0;
	if ((nice = getpriority(PRIO_PROCESS, pid)) == -1 && errno != 0)
		errExit("getpriority");
	printf("before nice(),  pid %ld progress's nice is %d\n", (pid == 0) ? (long)getpid() : (long)pid, nice);

	if (my_nice(pid, incr) == -1)
		errExit("nice");

	errno = 0;
	if ((nice = getpriority(PRIO_PROCESS, pid)) == -1 && errno != 0)
		errExit("getpriority");
	printf("after nice(), pid %ld progress's nice is %d\n", (pid == 0) ? (long)getpid() : (long)pid, nice);

	return 0;
}

int my_nice(pid_t pid, int incr)
{
	int nice;
	if (pid == 0)
		pid = getpid();

	errno = 0;
	if ((nice = getpriority(PRIO_PROCESS, pid)) == -1 && errno != 0)
		return -1;
	if (setpriority(PRIO_PROCESS, pid, incr + nice) == -1)
		return -1;
	return 0;
}
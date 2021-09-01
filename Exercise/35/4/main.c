#define _GNU_SOURCE

#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	cpu_set_t set;
	int pfd[2];
	char temp[30];
	int a = 0;
	struct tms tms;
	double start, curr;

	if (pipe(pfd) == -1)
		errExit("pipe");
	if (argc != 2 || strlen(argv[1]) != 1 || strstr("sd", argv[1]) == NULL)
	{
		printf("%s usage: <option>\n\
			   \t s - run on the same cpu\n\
			   \t d - run on the different cpu\n ",
			   argv[0]);
		exit(EXIT_FAILURE);
	}

	CPU_ZERO(&set);
	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	case 0: // child
		close(pfd[1]);
		if (argv[1][0] == 'd')
			CPU_SET(1, &set);
		else
			CPU_SET(0, &set);
		sched_setaffinity(0, sizeof(set), &set);

		if (times(&tms) == (clock_t)-1)
			return -1;
		start = ((double)tms.tms_stime + (double)tms.tms_utime) / sysconf(_SC_CLK_TCK); // 这个不同于CLOCKS_PER_SEC

		while (read(pfd[0], temp, 30) > 0)
		{
			++a;
			if (a % 100000 == 0)
				printf("have read 100000 times: %s\n", temp);
		}

		if (times(&tms) == (clock_t)-1)
			return -1;
		curr = ((double)tms.tms_stime + (double)tms.tms_utime) / sysconf(_SC_CLK_TCK) - start;
		if (argv[1][0] == 'd')
			printf("Run on different CPU cost time:\n");
		else
			printf("Run on same CPU cost time:\n");
		printf("\tpid: %ld, cpu time %lf secs\n", (long)getpid(), curr);

		_exit(EXIT_SUCCESS);
		break;

	default: // parent
		close(pfd[0]);
		CPU_SET(0, &set);
		sched_setaffinity(0, sizeof(set), &set);

		for (int i = 0; i < 1000000; ++i)
			if (write(pfd[1], "abcdefghijklmnopqrstuvwxyz", 26) != 26 * sizeof(char))
				errExit("write");
		break;
	}
	exit(EXIT_SUCCESS);
}
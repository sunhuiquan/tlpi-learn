#define _GNU_SOURCE

#include <sched.h>
#include <time.h>
#include <sys/times.h>
#include <tlpi_hdr.h>

int func();

int main(int argc, char *argv[])
{
	pid_t pid;
	struct sched_param sp;
	cpu_set_t set;

	CPU_ZERO(&set);
	CPU_SET(0, &set); // 只能在CPU核0跑，单核，这样才能体现出来之后的FIFO实时调度

	sched_setaffinity(0, sizeof(set), &set);

	sp.sched_priority = 1;
	if (sched_setscheduler(0, SCHED_FIFO, &sp) == -1)
		errExit("sched_setscheduler");

	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	case 0: // child
		if (func() == -1)
			errExit("func");
		_exit(EXIT_SUCCESS);
		break;

	default: // parent
		if (func() == -1)
			errExit("func");
		break;
	}
	exit(EXIT_SUCCESS);
}

int func()
{
	struct tms tms;
	double start, curr;
	int n = 1;
	struct sched_param param;

	if (sched_getparam(0, &param) == -1)
		return -1;
	if (param.sched_priority == SCHED_FIFO)
		printf("pid: %ld is SCHED_FIFO\n", (long)getpid());
	else if (param.sched_priority == SCHED_OTHER)
		printf("pid: %ld is SCHED_OTHER\n", (long)getpid());
	else
		return -1;

	if (times(&tms) == (clock_t)-1)
		return -1;
	start = ((double)tms.tms_stime + (double)tms.tms_utime) / sysconf(_SC_CLK_TCK); // 这个不同于CLOCKS_PER_SEC

	while (true)
	{
		if (times(&tms) == (clock_t)-1)
			return -1;
		curr = ((double)tms.tms_stime + (double)tms.tms_utime) / sysconf(_SC_CLK_TCK) - start;

		if (curr > 3)
			break;
		else if (curr - 0.25 * n >= 0)
		{
			printf("pid: %ld, cpu time %lf secs\n", (long)getpid(), curr);
			if (n % 4 == 0)
			{
				printf("pid %ld: is going to yield\n", (long)getpid());
				if (sched_yield() == -1)
					return -1;
				printf("pid %ld: come back\n", (long)getpid());
			}
			++n;
		}
	}
	return 0;
}

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	cpu_set_t set;

	if (argc != 2 || strlen(argv[1]) != 1 || strstr("sd", argv[1]) == NULL)
	{
		printf("%s usage: <option>\n"
			   "\t s - run on the same cpu\n"
			   "\t d - run on the different cpu\n");
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
		if (argv[1][0] == 'd')
			CPU_SET(1, &set);
		else
			CPU_SET(0, &set);
		sched_setaffinity(0, sizeof(set), &set);

		// to do
		_exit(EXIT_SUCCESS);
		break;

	default: // parent
		CPU_SET(0, &set);
		sched_setaffinity(0, sizeof(set), &set);

		// to do
		break;
	}
	exit(EXIT_SUCCESS);
}
#define _POSIX_C_SOURCE 199309
#define _XOPEN_SOURCE 600
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void
sigintHandler(int sig)
{
	return; /* Just interrupt nanosleep() */
}

int main(int argc, char *argv[])
{
	struct timeval start, finish;
	struct timespec request, curr;
	// struct timespec request, remain;
	struct sigaction sa;
	long sec, nsec;
	int s;

	if (argc != 3 || strcmp(argv[1], "--help") == 0)
		usageErr("%s secs nanosecs\n", argv[0]);

	clock_gettime(CLOCK_REALTIME, &request);
	request.tv_sec += getLong(argv[1], 0, "secs");
	request.tv_nsec += getLong(argv[2], 0, "nanosecs");

	/* Allow SIGINT handler to interrupt nanosleep() */

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigintHandler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		errExit("sigaction");

	if (gettimeofday(&start, NULL) == -1)
		errExit("gettimeofday");

	printf("%ld\n", (long)getpid());
	for (;;)
	{
		s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &request, NULL);
		// s = nanosleep(&request, &remain);
		if (s == -1 && errno != EINTR)
			errExit("nanosleep");

		if (gettimeofday(&finish, NULL) == -1)
			errExit("gettimeofday");
		printf("Slept for: %9.6f secs\n", finish.tv_sec - start.tv_sec +
											  (finish.tv_usec - start.tv_usec) / 1000000.0);

		if (s == 0)
			break; /* nanosleep() completed */

		clock_gettime(CLOCK_REALTIME, &curr);
		sec = (long)(request.tv_sec - curr.tv_sec);
		nsec = request.tv_nsec - curr.tv_nsec;
		if (nsec < 0)
		{
			sec -= 1;
			nsec += 1000000000;
		}
		printf("Remaining: %2ld.%09ld\n", sec, nsec);
		// request = remain; /* Next sleep is with remaining time */
	}

	printf("Sleep complete\n");
	exit(EXIT_SUCCESS);
}

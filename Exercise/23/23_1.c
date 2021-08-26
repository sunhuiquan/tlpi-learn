#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

unsigned int my_alarm(unsigned int seconds)
{
	struct itimerval new, old;
	new.it_value.tv_sec = seconds;
	new.it_value.tv_usec = 0;
	new.it_interval.tv_sec = 0;
	new.it_interval.tv_usec = 0;

	setitimer(ITIMER_REAL, &new, &old);
	return old.it_value.tv_sec;
}

int main()
{
	printf("%u\n", my_alarm(5));
	printf("%u\n", my_alarm(2));
	printf("%u\n", my_alarm(3));

	for (;;)
		;

	return 0;
}
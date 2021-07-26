#include <sys/inotify.h>
#include <stdio.h>

// struct inotify_event ie;

int main()
{
	printf("%d\n", inotify_init());

	return 0;
}
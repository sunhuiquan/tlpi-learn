#include <sys/inotify.h>
#include <limits.h>
#include "tlpi_hdr.h"

static void /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
	if (i->cookie > 0)
		printf("cookie =%4d; ", i->cookie);

	printf("mask = ");
	if (i->mask & IN_CREATE)
		printf("IN_CREATE ");
	if (i->mask & IN_DELETE)
		printf("IN_DELETE ");
	if (i->mask & IN_MOVED_FROM)
		printf("IN_MOVED_FROM ");
	if (i->mask & IN_MOVED_TO)
		printf("IN_MOVED_TO ");
	printf("\n");

	if (i->len > 0)
		printf("name = %s\n", i->name);
}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[])
{
	int inotifyFd, wd;
	char buf[BUF_LEN];
	ssize_t numRead;
	char *p;
	struct inotify_event *event;

	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("usage: %s pathname\n", argv[0]);

	inotifyFd = inotify_init(); /* Create inotify instance */
	if (inotifyFd == -1)
		errExit("inotify_init");

	wd = inotify_add_watch(inotifyFd, argv[1], IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
	if (wd == -1)
		errExit("inotify_add_watch");

	printf("Watching %s using wd %d\n", argv[1], wd);

	for (;;)
	{ /* Read events forever */
		numRead = read(inotifyFd, buf, BUF_LEN);
		if (numRead == 0)
			fatal("read() from inotify fd returned 0!");

		if (numRead == -1)
			errExit("read");

		/*FIXME: should use %zd here, and remove (long) cast */
		printf("Read %ld bytes from inotify fd\n", (long)numRead);

		/* Process all of the events in buffer returned by read() */

		for (p = buf; p < buf + numRead;)
		{
			event = (struct inotify_event *)p;
			displayInotifyEvent(event);

			p += sizeof(struct inotify_event) + event->len;
		}
	}

	exit(EXIT_SUCCESS);
}

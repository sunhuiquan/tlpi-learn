#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1000

char *currTime(const char *format);

int main(int argc, char *argv[])
{
	int fd, lock;
	const char *lname;

	if (argc < 3 || strcmp(argv[1], "--help") == 0 ||
		strchr("sx", argv[2][0]) == NULL)
		usageErr("%s file lock [sleep-time]\n"
				 "    'lock' is 's' (shared) or 'x' (exclusive)\n"
				 "        optionally followed by 'n' (nonblocking)\n"
				 "    'sleep-time' specifies time to hold lock\n",
				 argv[0]);

	lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
	if (argv[2][1] == 'n')
		lock |= LOCK_NB;

	fd = open(argv[1], O_RDONLY); /* Open file to be locked */
	if (fd == -1)
		errExit("open");

	lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";

	printf("PID %ld: requesting %s at %s\n", (long)getpid(), lname,
		   currTime("%T"));

	if (flock(fd, lock) == -1)
	{
		if (errno == EWOULDBLOCK)
			fatal("PID %ld: already locked - bye!", (long)getpid());
		else
			errExit("flock (PID=%ld)", (long)getpid());
	}

	printf("PID %ld: granted    %s at %s\n", (long)getpid(), lname,
		   currTime("%T"));

	sleep((argc > 3) ? getInt(argv[3], GN_NONNEG, "sleep-time") : 10);

	printf("PID %ld: releasing  %s at %s\n", (long)getpid(), lname,
		   currTime("%T"));
	if (flock(fd, LOCK_UN) == -1)
		errExit("flock");

	exit(EXIT_SUCCESS);
}

char *currTime(const char *format)
{
	static char buf[BUF_SIZE]; /* Nonreentrant */
	time_t t;
	size_t s;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	if (tm == NULL)
		return NULL;

	s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);

	return (s == 0) ? NULL : buf;
}

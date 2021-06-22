#include "./mmap_xfr.h"

int main(int argc, char *argv[])
{
	int semid, xfrs, bytes, fd;
	struct seg *seg_addr;
	void *addr;

	/* Get IDs for semaphore set and shared memory created by writer */

	semid = semget(SEM_KEY, 0, 0);
	if (semid == -1)
		errExit("semget");

	if ((fd = open(SHARED_FILE, O_RDONLY)) == -1)
		errExit("open");
	if ((addr = mmap(NULL, sizeof(struct seg), PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
		errExit("mmap");
	seg_addr = (struct seg *)addr;
	if (close(fd) == -1)
		errExit("close");

	/* Transfer blocks of data from shared memory to stdout */

	for (xfrs = 0, bytes = 0;; xfrs++)
	{
		if (reserveSem(semid, READ_SEM) == -1) /* Wait for our turn */
			errExit("reserveSem");

		if (seg_addr->cnt == 0) /* Writer encountered EOF */
			break;
		bytes += seg_addr->cnt;

		if (write(STDOUT_FILENO, seg_addr->buf, seg_addr->cnt) != seg_addr->cnt)
			fatal("partial/failed write");

		if (releaseSem(semid, WRITE_SEM) == -1) /* Give writer a turn */
			errExit("releaseSem");
	}

	/* Give writer one more turn, so it can clean up */

	if (releaseSem(semid, WRITE_SEM) == -1)
		errExit("releaseSem");

	fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
	exit(EXIT_SUCCESS);
}

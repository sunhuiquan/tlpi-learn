#include "./mmap_xfr.h"
#include "../../../tlpi-dist/svsem/semun.h"

int main(int argc, char *argv[])
{
	int semid, bytes, xfrs, fd;
	union semun dummy;
	void *addr;
	struct seg *seg_addr;

	/* Create set containing two semaphores; initialize so that
       writer has first access to shared memory. */

	semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
	if (semid == -1)
		errExit("semget");

	if (initSemAvailable(semid, WRITE_SEM) == -1)
		errExit("initSemAvailable");
	if (initSemInUse(semid, READ_SEM) == -1)
		errExit("initSemInUse");

	if ((fd = open(SHARED_FILE, O_CREAT | O_EXCL | O_RDWR, 0666)) == -1)
		errExit("open");
	if (ftruncate(fd, sizeof(struct seg)) == -1)
		errExit("ftruncate");
	if ((addr = mmap(NULL, sizeof(struct seg), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
		errExit("mmap");
	seg_addr = (struct seg *)addr;
	if (close(fd) == -1)
		errExit("close");

	/* Transfer blocks of data from stdin to shared memory */

	for (xfrs = 0, bytes = 0;; xfrs++, bytes += seg_addr->cnt)
	{
		if (reserveSem(semid, WRITE_SEM) == -1) /* Wait for our turn */
			errExit("reserveSem");

		seg_addr->cnt = read(STDIN_FILENO, seg_addr->buf, BUF_SIZE);
		if (seg_addr->cnt == -1)
			errExit("read");

		if (releaseSem(semid, READ_SEM) == -1) /* Give reader a turn */
			errExit("releaseSem");

		/* Have we reached EOF? We test this after giving the reader
           a turn so that it can see the 0 value in shmp->cnt. */

		if (seg_addr->cnt == 0)
			break;
	}

	/* Wait until reader has let us have one more turn. We then know
       reader has finished, and so we can delete the IPC objects. */

	if (reserveSem(semid, WRITE_SEM) == -1)
		errExit("reserveSem");
	if (semctl(semid, 0, IPC_RMID, dummy) == -1)
		errExit("semctl");

	if (unlink(SHARED_FILE) == -1)
		errExit("unlink");

	fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
	exit(EXIT_SUCCESS);
}

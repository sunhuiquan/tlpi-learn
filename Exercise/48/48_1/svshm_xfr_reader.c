#include "svshm_xfr.h"

int main(int argc, char *argv[])
{
    int semid, shmid, xfrs, bytes;
    struct shmseg *shmp;

    /* Get IDs for semaphore set and shared memory created by writer */

    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1)
        errExit("semget");

    shmid = shmget(SHM_KEY, 0, 0);
    if (shmid == -1)
        errExit("shmget");

    /* Attach shared memory read-only, as we will only read */

    shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void *)-1)
        errExit("shmat");

    /* Transfer blocks of data from shared memory to stdout */

    for (xfrs = 0, bytes = 0;; xfrs++)
    {
        if (waitForEventFlag(semid, READ_SEM) == -1)
            errExit("waitForEventFlag");

        if (shmp->cnt == 0) /* Writer encountered EOF */
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
            fatal("partial/failed write");

        if (setEventFlag(semid, WRITE_SEM) == -1)
            errExit("setEventFlag");
    }

    if (shmdt(shmp) == -1)
        errExit("shmdt");

    /* Give writer one more turn, so it can clean up */

    if (setEventFlag(semid, WRITE_SEM) == -1)
        errExit("setEventFlag");

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}

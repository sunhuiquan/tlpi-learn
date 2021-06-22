#include "poshm_xfr.h"

int main(int argc, char *argv[])
{
    int shmid, semid, xfrs, bytes;
    struct shmseg *shmp;

    /* Get IDs for semaphore set and shared memory created by writer */

    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1)
        errExit("semget");

    shmid = shm_open(SHM_PATHNAME, O_RDONLY, 0); // shm_open的mode不能省略，虽然打开的时候是忽略的
    if (shmid == -1)
        errExit("shm_open");

    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, shmid, 0);
    if ((void *)shmp == MAP_FAILED)
        errExit("mmap");

    if (close(shmid) == -1)
        errExit("close");

    /* Transfer blocks of data from shared memory to stdout */

    for (xfrs = 0, bytes = 0;; xfrs++)
    {
        if (reserveSem(semid, READ_SEM) == -1) /* Wait for our turn */
            errExit("reserveSem");

        if (shmp->cnt == 0) /* Writer encountered EOF */
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
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

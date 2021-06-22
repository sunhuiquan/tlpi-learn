#include "../../tlpi-dist/svsem/semun.h" /* Definition of semun union */
#include "poshm_xfr.h"

int main(int argc, char *argv[])
{
    int shmid, semid, bytes, xfrs;
    union semun dummy;
    struct shmseg *addr;

    /* Create set containing two semaphores; initialize so that
       writer has first access to shared memory. */

    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1)
        errExit("semget");

    if (initSemAvailable(semid, WRITE_SEM) == -1)
        errExit("initSemAvailable");
    if (initSemInUse(semid, READ_SEM) == -1)
        errExit("initSemInUse");

    /* Create shard memory; attach at address chosen by system */
    shmid = shm_open(SHM_PATHNAME, O_CREAT | O_EXCL | O_RDWR, OBJ_PERMS);
    if (shmid == -1)
        errExit("shm_open");

    if (ftruncate(shmid, sizeof(struct shmseg)) == -1)
        errExit("ftruncate");

    addr = mmap(NULL, sizeof(struct shmseg), PROT_WRITE, MAP_SHARED, shmid, 0);
    if ((void *)addr == MAP_FAILED)
        errExit("mmap");

    if (close(shmid) == -1)
        errExit("close");

    /* Transfer blocks of data from stdin to shared memory */

    for (xfrs = 0, bytes = 0;; xfrs++, bytes += addr->cnt)
    {
        if (reserveSem(semid, WRITE_SEM) == -1) /* Wait for our turn */
            errExit("reserveSem");

        addr->cnt = read(STDIN_FILENO, addr->buf, BUF_SIZE);
        if (addr->cnt == -1)
            errExit("read");

        if (releaseSem(semid, READ_SEM) == -1) /* Give reader a turn */
            errExit("releaseSem");

        /* Have we reached EOF? We test this after giving the reader
           a turn so that it can see the 0 value in shmp->cnt. */

        if (addr->cnt == 0)
            break;
    }

    /* Wait until reader has let us have one more turn. We then know
       reader has finished, and so we can delete the IPC objects. */

    if (reserveSem(semid, WRITE_SEM) == -1)
        errExit("reserveSem");

    if (semctl(semid, 0, IPC_RMID, dummy) == -1)
        errExit("semctl");

    if (shm_unlink(SHM_PATHNAME) == -1)
        errExit("shm_unlink");

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}

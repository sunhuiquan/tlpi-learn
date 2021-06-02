#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include "../../tlpi-dist/time/curr_time.h"
#include "tlpi_hdr.h"
#include "semun.h"

int main(int argc, char *argv[])
{
    pid_t childPid;
    int semid;
    union semun arg;
    struct sembuf sop;

    /* IPC_PRIVATE must already have IPC_CREAT and IPC_EXCL */
    semid = semget(IPC_PRIVATE, 1, S_IWUSR | S_IRUSR);
    if (semid == -1)
        errExit("semget");

    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1)
        errExit("semctl");

    switch (childPid = fork())
    {
    case -1:
        errExit("fork");

    case 0: /* Child */
        printf("[%s %ld] Child started - doing some work\n",
               currTime("%T"), (long)getpid());
        sleep(2); /* Simulate time spent doing some work */

        printf("[%s %ld] Child about to inform parent\n",
               currTime("%T"), (long)getpid());
        sop.sem_flg = 0;
        sop.sem_num = 0;
        sop.sem_op = -1;
        if (semop(semid, &sop, 1) == -1)
            errExit("semop");

        /* Now child can do other things... */
        sleep(2);

        _exit(EXIT_SUCCESS);

    default: /* Parent */
        printf("[%s %ld] Parent about to wait for signal\n",
               currTime("%T"), (long)getpid());
        sop.sem_flg = 0;
        sop.sem_num = 0;
        sop.sem_op = 0;
        if (semop(semid, &sop, 1) == -1)
            errExit("semop");

        printf("[%s %ld] Parent got work\n", currTime("%T"), (long)getpid());

        if (semctl(semid, 0, IPC_RMID) == -1)
            errExit("semctl RMID");

        exit(EXIT_SUCCESS);
    }
}

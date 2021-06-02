#include <sys/stat.h>
#include <sys/sem.h>
#include "tlpi_hdr.h"
#include "semun.h"

int main(int argc, char *argv[])
{
    int semid;
    union semun arg;
    struct sembuf sop;
    printf("pid: %ld\n", (long)getpid());

    if ((semid = semget(IPC_PRIVATE, 1, S_IWUSR | S_IRUSR)) == -1)
        errExit("semget");

    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg) == -1)
        errExit("semctl SETVAL");

    sop.sem_flg = SEM_UNDO;
    sop.sem_num = 0;
    sop.sem_op = 10;
    if (semop(semid, &sop, 1) == -1)
        errExit("semop");

    printf("a SEMPID: %d\n", semctl(semid, 0, GETPID));
    sleep(10);
    printf("a SEMPID: %d\n", semctl(semid, 0, GETPID));

    return 0;
}

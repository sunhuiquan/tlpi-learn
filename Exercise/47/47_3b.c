#include <sys/stat.h>
#include <sys/sem.h>
#include <string.h>
#include "tlpi_hdr.h"
#include "semun.h"

int main(int argc, char *argv[])
{
    int semid = atoi(argv[1]);
    struct sembuf sop;

    sop.sem_flg = 0;
    sop.sem_num = 0;
    sop.sem_op = 1;
    if (semop(semid, &sop, 1) == -1)
        errExit("semop");
    sleep(20);
    printf("b SEMPID: %d\n", semctl(semid, 0, GETPID));
    return 0;
}

#include <sys/types.h>
#include <sys/sem.h>
#include "../semun.h"
#include "event_flag.h"

#include <stdio.h>

int setEventFlag(int semId, int semNum)
{
    union semun arg;

    arg.val = 0;
    return semctl(semId, semNum, SETVAL, arg);
}

int clearEventFlag(int semId, int semNum)
{
    union semun arg;

    arg.val = 1;
    return semctl(semId, semNum, SETVAL, arg);
}

int waitForEventFlag(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = 0;
    sops.sem_flg = 0;

    if (semop(semId, &sops, 1) == -1)
        return -1;

    return 0;
}

int getFlagState(int semId, int semNum)
{
    return semctl(semId, 0, GETVAL);
}

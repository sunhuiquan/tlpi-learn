#include <tlpi_hdr.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "binary_sems.h"

int main()
{
    int semid;
    pid_t pid;

    if ((semid = semget(IPC_PRIVATE, 1, S_IWUSR | S_IRUSR)) == -1)
        errExit("semid");
    if (initSemInUse(semid, 0) == -1)
        errExit("initSemInUse");

    switch (pid = fork())
    {
    case -1:
        errExit("fork");

    case 0:
        //child
        printf("child holds resouce at first\n");
        sleep(2);
        printf("child release resouce and wait get resourse again\n");
        if (releaseSem(semid, 0) == -1)
            errExit("releaseSem");

        if (reserveSem(semid, 0) == -1)
            errExit("reserveSem");
        sleep(2);
        printf("child hold again\n");
        _exit(EXIT_SUCCESS);
    }

    // parent
    if (reserveSem(semid, 0) == -1)
        errExit("reserveSem");
    printf("parent get resource\n");

    sleep(2);
    printf("parent release resouce\n");
    if (releaseSem(semid, 0) == -1)
        errExit("reserveSem");

    return 0;
}

#include <tlpi_hdr.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include "event_flag.h"

int main()
{
    pid_t pid;
    int semid;
    char path[1024];

    semid = semget(IPC_PRIVATE, 1, S_IWUSR | S_IRUSR);
    if (semid == -1)
        errExit("semid");

    clearEventFlag(semid, 0);

    switch (pid = fork())
    {
    case -1:
        errExit("fork");

    case 0:
        //child
        printf("child holds resouce at first\n");
        sleep(2);
        printf("child release resouce and wait get resourse again\n");
        setEventFlag(semid, 0);

        waitForEventFlag(semid, 0);
        sleep(2);
        printf("child hold again\n");
        _exit(EXIT_SUCCESS);
    }

    // parent
    waitForEventFlag(semid, 0);
    printf("parent get resource\n");

    sleep(2);
    printf("parent release resouce\n");
    setEventFlag(semid, 0);

    unlink(path);
    return 0;
}

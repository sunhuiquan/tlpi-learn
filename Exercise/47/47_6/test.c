#include <tlpi_hdr.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include "binary_fifo.h"

int main()
{
    pid_t pid;
    char path[1024];

    sprintf(path, "/tmp/fifo-%d", getpid());
    if (mkfifo(path, 0666) == -1)
        errExit("mkfifo");

    int rfd;
    switch (pid = fork())
    {
    case -1:
        errExit("fork");

    case 0:
        //child
        printf("child holds resouce at first\n");
        sleep(2);
        printf("child release resouce and wait get resourse again\n");
        if (release(path) == -1)
            errExit("release");

        sleep(1);
        printf("a\n");
        if (reserveNB(path) == -1)
        {
            if (errno == ENXIO)
                printf("try no block fail and then block wait\n");
            else
                errExit("reserveNB");
        }

        if (reserve(path) == -1)
            errExit("reserve");
        sleep(2);
        printf("child hold again\n");
        _exit(EXIT_SUCCESS);
    }

    // parent
    if (reserve(path) == -1)
        errExit("reserve");
    printf("parent get resource\n");

    sleep(2);
    printf("parent release resouce\n");
    if (release(path) == -1)
        errExit("reserve");

    unlink(path);
    return 0;
}

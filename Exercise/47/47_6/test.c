#include <tlpi_hdr.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include "binary_fifo.h"

#include <fcntl.h>

int main()
{
    pid_t pid;
    int rfd, wfd;
    char path[1024];

    sprintf(path, "/tmp/fifo-%d", getpid());
    if (mkfifo(path, 0666) == -1)
        errExit("mkfifo");
    init(path, &rfd, &wfd);
    unlink(path);

    switch (pid = fork())
    {
    case -1:
        errExit("fork");

    case 0:
        //child
        printf("child holds resouce at first\n");
        sleep(2);
        printf("child release resouce and wait get resourse again\n");
        if (release(wfd) == -1)
            errExit("release");

        sleep(1);
        if (reserveNB(rfd) == -1)
        {
            if (errno == EAGAIN)
                printf("try no block fail and then block wait\n");
            else
                errExit("reserveNB");
        }
        sleep(1); // 我的实现和信号量实现的语义不同，
        // 因为信号量实现可以直接通知到其他进程的semop不再阻塞了，
        // 然后把信号量改了，而我这里只能等到切换到那个进程才可以体现
        if (reserve(rfd) == -1)
            errExit("reserve");
        sleep(2);
        printf("child hold again\n");
        _exit(EXIT_SUCCESS);
    }

    // parent
    if (reserve(rfd) == -1)
        errExit("reserve");
    printf("parent get resource\n");

    sleep(2);
    printf("parent release resouce\n");
    if (release(wfd) == -1)
        errExit("reserve");

    return 0;
}

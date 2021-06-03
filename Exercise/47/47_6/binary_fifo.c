#include "binary_fifo.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

static int rfd = -1;

// 这里的阻塞被中断后是否重启取决于SA_RESTART标志
int reserve(const char *fifo)
{
    int wfd = open(fifo, O_WRONLY);
    printf("open rfd %d\n", wfd);
    if (wfd == -1)
        return -1;

    close(rfd);
    close(wfd);
    printf("close rfd %d\n", rfd);
    printf("close wfd %d\n", wfd);
    return 0;
}

int release(const char *fifo)
{
    rfd = open(fifo, O_RDONLY | O_NONBLOCK);
    printf("open rfd %d\n", rfd);
    if (rfd == -1)
        return -1;
    return 0;
}

int reserveNB(const char *fifo)
{
    int wfd = open(fifo, O_WRONLY | O_NONBLOCK);
    printf("open rfd %d\n", wfd);
    if (wfd == -1)
        return -1;

    close(rfd);
    close(wfd);
    printf("close rfd %d\n", rfd);
    printf("close wfd %d\n", wfd);
    return 0;
}
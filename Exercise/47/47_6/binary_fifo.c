#include "binary_fifo.h"
#include <sys/stat.h>
#include <fcntl.h>

static int rfd;

// 这里的阻塞被中断后是否重启取决于SA_RESTART标志
int reserve(const char *fifo)
{
    int wfd = open(fifo, O_WRONLY);
    if (wfd == -1)
        return -1;

    close(rfd);
    close(wfd);
    return 0;
}

int release(const char *fifo)
{
    rfd = open(fifo, O_RDONLY | O_NONBLOCK);
    if (rfd == -1)
        return -1;
    return 0;
}

int reserveNB(const char *fifo)
{
    int wfd = open(fifo, O_WRONLY | O_NONBLOCK);
    if (wfd == -1)
        return -1;

    close(rfd);
    close(wfd);
    return 0;
}
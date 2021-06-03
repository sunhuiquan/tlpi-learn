#include "binary_fifo.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int init(const char *fifo, int *rfd, int *wfd)
{
    int flags;

    *rfd = open(fifo, O_RDONLY | O_NONBLOCK);
    if (*rfd == -1)
        return -1;
    *wfd = open(fifo, O_WRONLY);
    if (*wfd == -1)
        return -1;

    flags = fcntl(*rfd, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(*rfd, F_SETFL, flags);

    return 0;
}

// 这里的阻塞被中断后是否重启取决于SA_RESTART标志
int reserve(int rfd)
{
    char x;
    if (read(rfd, &x, 1) != 1)
        return -1;
    return 0;
}

int reserveNB(int rfd)
{
    char x;
    int flags;

    flags = fcntl(rfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(rfd, F_SETFL, flags);

    ssize_t n = read(rfd, &x, 1);

    flags = fcntl(rfd, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(rfd, F_SETFL, flags);

    if (n == -1)
        return -1;

    return 0;
}

int release(int wfd)
{
    if (write(wfd, "a", 1) != 1)
        return -1;
    return 0;
}
#include <tlpi_hdr.h>
#include <fcntl.h>

int my_dup(int fd);
int my_dup2(int fd, int fd2);

int main()
{
    /* An easy test */
    // test-1
    int fd;
    if ((fd = my_dup(1)) < 0)
        printf("my_dup error: %s\n", strerror(errno));
    else
        write(fd, "a\n", 2);

    // test-2
    // 这里是故意错误作为测试，所以不exit
    close(1000); // to ensure fd 1000 is closed
    if ((fd = my_dup(1000)) < 0)
        printf("my_dup error: %s\n", strerror(errno));
    else
        write(fd, "b\n", 2);

    // test-3
    if ((fd = my_dup2(1, 0)) < 0)
        printf("my_dup2 error: %s\n", strerror(errno));
    else
        write(0, "c\n", 2);

    // test-4
    close(1000);
    if ((fd = my_dup2(1000, 0)) < 0)
        printf("my_dup2 error: %s\n", strerror(errno));
    else
        write(0, "d\n", 2);

    return 0;
}

int my_dup(int fd)
{
    int new_fd;
    // fcntl 返回大于等于第三个参数的最小值，所以dup等价于从0开始
    if ((new_fd = fcntl(fd, F_DUPFD, 0)) < 0)
        return -1;
    // 不要exit，这又不是致命的错误，只是个函数错了，
    // 调用的得到错误的返回值好做相应的处理而已
    return new_fd;
}

int my_dup2(int fd, int fd2)
{
    if (fd == fd2)
    {
        // 虽然fcntl用F_DUPFD的时候可以知道fd错误的情况，
        // 不过因为这里不复制，所以F_GETFL
        if (fcntl(fd, F_GETFL) != -1 || errno != EBADF)
            return fd;
    }

    close(fd2);
    if (fcntl(fd, F_DUPFD, fd2) < 0)
        return -1;
    return fd2;
}
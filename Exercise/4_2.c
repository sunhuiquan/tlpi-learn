#include "tlpi_hdr.h"
#include <fcntl.h>
#include <unistd.h>
#include <fs.h>
#include <sys/types.h>

// 生成一个空洞文件
// int main()
// {
//     int fd = open("aaa", O_CREAT | O_WRONLY | O_TRUNC, 666);
//     write(fd, "a", 1);
//     lseek(fd, 1000000, SEEK_END);
//     for (int i = 0; i < 1000000; ++i)
//         write(fd, "\0", 1);
//     write(fd, "b", 1);
//     return 0;
// }

int main(int argc, char *argv[])
{
    char temp;
    int fd = open("aaa", O_RDONLY);
    if (fd < 0)
        exit(1);
    // 只有O_CREAT才需要写权限参数
    int fd2 = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 666);
    if (fd2 < 0)
        exit(1);
    ssize_t n = 0;
    while ((n = read(fd, &temp, 1)) > 0)
    {
        if (temp == 0)
        {
            if (lseek(fd2, SEEK_CUR, 1) < 0)
                exit(1);
        }
        else if (write(fd2, &temp, 1) != 1)
            exit(1);
    }
    if (n < 0)
        exit(1);

    if (close(fd) == -1)
        exit(1);
    if (close(fd2) == -1)
        exit(1);

    return 0;
}

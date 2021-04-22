#include <tlpi_hdr.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <stdlib.h>

ssize_t myreadv(int fd, const struct iovec *iovec, int count);
ssize_t mywritev(int fd, const struct iovec *iovec, int count);

int main()
{
    // char a[2] = "ab";
    // char b[2] = "c";
    // printf("%s\n", a); // abc 因为没2大小放不下'\0'
    // // overflow for "ab" is {'a'，'b'，'\0'}
    // printf("%s\n", b); // b

    struct iovec iovec[2];
    char b1[10] = "abcdefghij"; // ""字符串格式给字符数组赋值会自带一个'\0',这里没放入数组中
    char b2[5] = "abc";
    struct iovec iovec2[2];
    char b3[15];
    char b4[5]; // useless here
    int fd;

    iovec[0].iov_base = b1;
    iovec[0].iov_len = sizeof(b1);
    iovec[1].iov_base = b2;
    iovec[1].iov_len = 3;
    iovec2[0].iov_base = b3;
    iovec2[0].iov_len = sizeof(b3);
    iovec2[1].iov_base = b4;
    iovec2[1].iov_len = sizeof(b4);

    if ((fd = open("5_7file", O_CREAT | O_WRONLY | O_TRUNC, 0666)) < 0)
        printf("open error: %s\n", strerror(errno));
    if (mywritev(fd, iovec, 2) != iovec[0].iov_len + iovec[1].iov_len)
        printf("writev error: %s\n", strerror(errno));
    close(fd); // 刷新确保写入文件，不过open用同步选项加上lseek()也可以实现

    memset(b3, 0, sizeof(b3));
    memset(b4, 0, sizeof(b4));
    if ((fd = open("5_7file", O_RDONLY)) < 0)
        printf("open error: %s\n", strerror(errno));
    if (myreadv(fd, iovec2, 2) != iovec[0].iov_len + iovec[1].iov_len)
        printf("readv error: %s\n", strerror(errno));

    // 挨个char打印是因为缓冲区有没有哨兵，而且这又不一定是char类型，00可能只是一个
    // 值的01 00 00 00(小段int 1)的部分而已，我这里是知道我写的是char，所以用char读
    // 罢了，又不一定是char，当然不一定是'\0'了，而且没有意义
    size_t sz = sizeof(b3);
    for (size_t i = 0; i < sz; ++i)
    {
        printf("%c", b3[i]);
    }
    sz = sizeof(b4);
    for (size_t i = 0; i < sz; ++i)
    {
        printf("%c", b4[i]);
    }
    printf("\n");

    return 0;
}

// struct iovec
// {
//     void *iov_base; /* Pointer to data.  */
//     size_t iov_len; /* Length of data.  */
// };

ssize_t myreadv(int fd, const struct iovec *iovec, int count)
{
    ssize_t n;
    ssize_t rest_buffer; // 单个缓冲区剩余的空间
    ssize_t total_read = 0;

    for (int i = 0; i < count; ++i)
    {
        rest_buffer = iovec[i].iov_len;
        // 因为要考虑比如终端，比如网络I/O等等情况，不等于要求的数值也没读到头的情况外也不一定是错误
        while ((n = read(fd, iovec[i].iov_base, rest_buffer)) > 0)
        {
            total_read += n;
            rest_buffer -= n;
            if (rest_buffer < 0)
                break;
        }
        if (n < 0) // 返回错误
            return -1;
        if (n == 0) // 返回读完
            return total_read;
    }
    // 返回缓冲区用完
    return total_read;
}

ssize_t mywritev(int fd, const struct iovec *iovec, int count)
{
    ssize_t total_write = 0;
    for (int i = 0; i < count; ++i)
    {
        if (write(fd, iovec[i].iov_base, iovec[i].iov_len) != iovec[i].iov_len)
            return -1;
        total_write += iovec[i].iov_len;
    }
    return total_write;
}
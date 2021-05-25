#include <tlpi_hdr.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    int fd, fd2;
    struct rlimit rl, set_rl;
    char buf[150];
    memset(buf, 'a', 150);
    buf[149] = '\0';
    printf("%s\n", buf);

    if (getrlimit(RLIMIT_FSIZE, &rl) == -1)
        errExit("getrlimit");
    printf("soft limit: %lu\nhard limit: %lu\n", (unsigned long)rl.rlim_cur, (unsigned long)rl.rlim_max);

    if ((fd = open("aaatemp.txt", O_CREAT | O_RDWR, 0666)) == -1)
        errExit("open");

    if (write(fd, buf, 150) != 150)
        errExit("write");

    set_rl.rlim_cur = 100;
    if (setrlimit(RLIMIT_FSIZE, &set_rl) == -1)
        errExit("setrlimit");
    printf("\nafter set new limit\n");
    if (getrlimit(RLIMIT_FSIZE, &rl) == -1)
        errExit("getrlimit");
    printf("soft limit: %lu\nhard limit: %lu\n", (unsigned long)rl.rlim_cur, (unsigned long)rl.rlim_max);

    if ((fd2 = open("bbbtemp.txt", O_CREAT | O_RDWR, 0666)) == -1)
        errExit("open");

    if (write(fd2, buf, 150) != 150)
    {
        printf("%s\n", strerror(errno));
        system("ls -l bbbtemp.txt");
    }

    close(fd);
    close(fd2);

    return 0;
}
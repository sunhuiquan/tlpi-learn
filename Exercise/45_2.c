/* An easy way of implementing ftok() */
#include <sys/ipc.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <tlpi_hdr.h>

typedef int key_t;
key_t myftok(const char *pathname, int proj_id)
{
    struct stat info;
    if (stat(pathname, &info) == -1)
        return -1;

    return (int)(((proj_id & 0xff) << 24) +
                 (((int)info.st_dev & 0xff) << 16) + ((int)info.st_ino & 0xffff));
}

int main()
{
    key_t key = ftok("45_2.c", 2);
    printf("%08x\n", (int)key);
    key = myftok("45_2.c", 2);
    printf("%08x\n", (int)key);

    return 0;
}
#include <sys/ipc.h>
#include <sys/stat.h>
#include <stdio.h>

int main()
{
    struct stat buf;
    key_t key = ftok("45_1", 2);
    printf("%08x\n", (int)key);
    stat("45_1", &buf);
    printf("%02x\n", 2 & 0xff);
    printf("%02x\n", (int)buf.st_dev & 0xff);
    printf("%04x\n", (int)buf.st_ino & 0xffff);

    return 0;
}
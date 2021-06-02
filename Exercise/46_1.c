#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>

int main()
{
    printf("%d\n", msgget(0x4b01252, IPC_CREAT | S_IRUSR | S_IWUSR));

    return 0;
}
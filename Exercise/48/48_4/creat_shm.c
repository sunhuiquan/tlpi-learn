#include <sys/shm.h>
#include <tlpi_hdr.h>

int main()
{
    if (shmget(IPC_PRIVATE, 1024, 0666) == -1)
        errExit("shmget");

    return 0;
}
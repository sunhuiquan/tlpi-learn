#include <sys/stat.h>
#include <tlpi_hdr.h>
#include <fcntl.h>

int main()
{
    if (mkfifo("fifo", 0666) == -1)
        errExit("mkfifo");

    // if (open("fifo", O_RDONLY) == -1)
    //     errExit("open");
    // if (open("fifo", O_RDONLY) == -1)
    //     errExit("open");

    if (open("fifo", O_RDONLY | O_NONBLOCK) == -1)
        errExit("open");
    // if (open("fifo", O_WRONLY | O_NONBLOCK) == -1)
    //     errExit("open");
    if (open("fifo", O_WRONLY) == -1)
        errExit("open");

    printf("end\n");
    return 0;
}
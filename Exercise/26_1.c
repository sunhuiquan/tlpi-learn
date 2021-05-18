#include <unistd.h>
#include <tlpi_hdr.h>

int main()
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        errExit("fork");

    case 0:
        sleep(2);
        printf("%ld\n", (long)getppid());
        _exit(EXIT_FAILURE);

    default:
        exit(EXIT_SUCCESS);
    }

    return 0;
}
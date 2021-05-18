#include <tlpi_hdr.h>
#include <signal.h>

int main()
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        errExit("fork");

    case 0:
        raise(SIGABRT);
        errExit("raise");

    default:
        break;
    }

    return 0;
}
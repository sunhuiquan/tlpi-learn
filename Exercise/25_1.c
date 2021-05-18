#include <tlpi_hdr.h>
#include <wait.h>

int main()
{
    int status;
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        errExit("fork");

    case 0:
        exit(-1);

    default:
        wait(&status);
    }

    if (WIFEXITED(status))
        printf("%d\n", WEXITSTATUS(status));

    return 0;
}
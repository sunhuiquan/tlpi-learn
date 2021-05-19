#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

void sigchld_handler(int sig)
{
    printf("SIGCHLD is caught\n");
}

int main(void)
{
    pid_t pid;
    int status;
    sigset_t mask;
    struct sigaction sa;
    char cmd[20] = "ps | grep a.out";

    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    sa.sa_flags = 0;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    pid = fork();
    switch (pid)
    {
    case -1:
        errExit("fork");

    case 0:
        _exit(EXIT_SUCCESS);

    default:
        system(cmd);
        printf("\n");
        if (wait(&status) == -1)
            errExit("wait");

        system(cmd);
        sleep(3); // to make things easy to see

        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        exit(EXIT_SUCCESS);
    }
}
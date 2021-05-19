#include <sys/wait.h>
#include "tlpi_hdr.h"

void printWaitidStatus(const char *msg, const siginfo_t *info)
{
    if (msg != NULL)
        printf("%s", msg);

    int status = info->si_status; // It's a normal value, not the same as wait's status format

    if (info->si_code == CLD_EXITED)
    {
        printf("child exited, status=%d\n", status);
    }
    else if (info->si_code == CLD_KILLED)
    {
        printf("child killed by signal %d\n",
               status - 127);
    }
    else if (info->si_code == CLD_STOPPED)
    {
        printf("child stopped by signal %d (%s)\n",
               status, strsignal(status));

#ifdef WIFCONTINUED /* SUSv3 has this, but older Linux versions and \
                   some other UNIX implementations don't */
    }
    else if (info->si_code == CLD_CONTINUED)
    {
        printf("child continued\n");
#endif
    }
    else
    { /* Should never happen */
        printf("what happened to this child? (status=%x)\n",
               (unsigned int)status);
    }
}

int main(int argc, char *argv[])
{
    siginfo_t info;
    info.si_signo = SIGCHLD;
    pid_t childPid;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [exit-status]\n", argv[0]);

    switch (fork())
    {
    case -1:
        errExit("fork");

    case 0: /* Child: either exits immediately with given
                           status or loops waiting for signals */
        printf("Child started with PID = %ld\n", (long)getpid());
        if (argc > 1) /* Status supplied on command line? */
            exit(getInt(argv[1], 0, "exit-status"));
        else /* Otherwise, wait for signals */
            for (;;)
                pause();
        exit(EXIT_FAILURE); /* Not reached, but good practice */

    default: /* Parent: repeatedly wait on child until it
                           either exits or is terminated by a signal */
        for (;;)
        {
            childPid = waitid(P_ALL, 0, &info, WEXITED | WSTOPPED
#ifdef WCONTINUED
                                                   | WCONTINUED
#endif
            );
            if (childPid == -1)
                errExit("waitpid");

            printf("child exited with %d\n", info.si_status);

            if (info.si_code == CLD_EXITED || info.si_code == CLD_KILLED)
                exit(EXIT_SUCCESS);
        }
    }
}

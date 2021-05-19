#include <signal.h>
#include <libgen.h> /* For basename() declaration */
#include "tlpi_hdr.h"

#define CMD_SIZE 200

int get_sigchld = 0;

void sigchld_handler(int sig)
{
    get_sigchld = 1;
}

int main(int argc, char *argv[])
{
    char cmd[CMD_SIZE];
    pid_t childPid;

    setbuf(stdout, NULL); /* Disable buffering of stdout */

    printf("Parent PID=%ld\n", (long)getpid());

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigchld_handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    switch (childPid = fork())
    {
    case -1:
        errExit("fork");

    case 0: /* Child: immediately exits to become zombie */
        signal(SIGCHLD, SIG_DFL);
        printf("Child (PID=%ld) exiting\n", (long)getpid());
        _exit(EXIT_SUCCESS);

    default:                 /* Parent */
        while (!get_sigchld) // To ignore the other signals
            pause();

        snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
        system(cmd); /* View zombie child */

        /* Now send the "sure kill" signal to the zombie */

        if (kill(childPid, SIGKILL) == -1)
            errMsg("kill");
        sleep(3); /* Give child a chance to react to signal */
        printf("After sending SIGKILL to zombie (PID=%ld):\n", (long)childPid);
        system(cmd); /* View zombie child again */

        exit(EXIT_SUCCESS);
    }
}

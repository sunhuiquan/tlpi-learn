#include <tlpi_hdr.h>
#include <signal.h>

void handler(int sig)
{
    printf("caught a signal %d\n", sig);
}

int main()
{
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESETHAND;
    // act.sa_handler = SIG_IGN; // sa_handler is a macro
    act.sa_handler = handler;
    if (sigaction(SIGTERM, &act, NULL) == -1)
        errExit("sigaction");

    while (1)
    {
        pause();
    }

    return 0;
}
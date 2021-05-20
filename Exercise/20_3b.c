#include <tlpi_hdr.h>
#include <signal.h>

void handler(int sig)
{
    printf("a\n");
    sleep(3);
    printf("b\n");
}

int main()
{
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    // act.sa_flags = 0;
    act.sa_handler = handler;
    if (sigaction(SIGTERM, &act, NULL) == -1)
        errExit("sigaction");

    while (1)
    {
        pause();
    }

    return 0;
}
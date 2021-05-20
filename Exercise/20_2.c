#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;

    if (sigaction(SIGTERM, &act, NULL) == -1)
    {
        printf("sigaction failed\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        pause();
    }

    return 0;
}
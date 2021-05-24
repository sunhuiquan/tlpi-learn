#include <signal.h>
#include <tlpi_hdr.h>

void handler(int sig)
{
    printf("%d\n", sig); // it's unsafe
}

int main()
{
    sigset_t all_set, empty_set;
    struct sigaction act;

    sigfillset(&act.sa_mask); // to ensure the printf order is just the same as the handler invoking
    act.sa_flags = 0;
    act.sa_handler = handler;

    sigfillset(&all_set);
    if (sigprocmask(SIG_SETMASK, &all_set, NULL) == -1)
        errExit("sigprocmask");

    for (int i = 1; i < 32; ++i) // 0 is empty signal
        if (i != SIGSTOP && i != SIGKILL)
        // expert these two signals, we can change other signals' handle effort
        {
            // printf("%d\n", i);
            if (sigaction(i, &act, NULL) == -1)
                errExit("sigaction");
        }

    // for (int i = SIGRTMIN; i <= SIGRTMAX; ++i)
    //     if (i != SIGSTOP && i != SIGKILL)
    //     {
    //         if (sigaction(i, &act, NULL) == -1)
    //             errExit("sigaction");
    //     }

    for (int i = 1; i < 32; ++i)
        if (i != SIGSTOP && i != SIGKILL)
            raise(i);

    // for (int i = SIGRTMIN; i <= SIGRTMAX; ++i)
    //     if (i != SIGSTOP && i != SIGKILL)
    //         raise(i);

    sigemptyset(&empty_set);
    if (sigprocmask(SIG_SETMASK, &empty_set, NULL) == -1)
        errExit("sigprocmask");

    return 0;
}
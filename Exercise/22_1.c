#include <tlpi_hdr.h>
#include <signal.h>

void sigcont_handler(int sig)
{
    printf("Invoke the sigcont's handler.\n"); // It's unsafe
}

int main()
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sigcont_handler;
    if (sigaction(SIGCONT, &act, NULL) == -1)
        errExit("sigaction");

    sigset_t new_set, old_set;
    sigemptyset(&new_set);
    sigaddset(&new_set, SIGCONT);
    sigprocmask(SIG_BLOCK, &new_set, &old_set);
    printf("%ld waiting SIGSTOP\n", (long)getpid());
    sleep(30);

    printf("Process is getting continue.\n");
    printf("Unlock SIGCONT.\n");
    sigprocmask(SIG_SETMASK, &old_set, NULL);

    return 0;
}
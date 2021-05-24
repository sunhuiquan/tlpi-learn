/**
 * A easy way of implementing abort() function.
 */
#include <signal.h>
#include <assert.h>
#include <tlpi_hdr.h>

void mabort(void);

#define NORMAL
// #define PENDING
// #define IGNORE
// #define CAPTURE

void sigabrt_handler(int sig)
{
    // do nothing
}

int main()
{
#ifdef NORMAL
    mabort();
#endif

#ifdef PENDING
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGABRT);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        errExit("sigprocmask");
    mabort();
#endif

#ifdef IGNORE
    signal(SIGABRT, SIG_IGN
    mabort();
#endif

#ifdef CAPTURE
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sigabrt_handler;
    if (sigaction(SIGABRT, &act, NULL) == -1)
        errExit("sigaction");

    mabort();
#endif

    assert(1 == 2);
    return 0;
}

void mabort(void)
{
    /* Unblock the sigabrt signal */
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGABRT);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    raise(SIGABRT); // if success, process will terminate here and generate a core-dump file

    /* SIG_IGN or the signal was caught */

    signal(SIGABRT, SIG_DFL);
    // though signal() is not portable for setting hander functions,
    // it's portable for setting SIG_DEF and SIG_IGN

    raise(SIGABRT);
}

/**
 * These are System V signal API
 * need define _XOPEN_SOURCE 500 first
 * typedef void (*__sighandler_t)(int);
 * __sighandler_t sigset(int __sig, __sighandler_t __disp)
 * int sighold(int __sig)
 * int sigrelse(int __sig)
 * int sigignore(int __sig)
 * int sigpause(int __sig)
 */

/**
 * #define	SIG_ERR	 ((__sighandler_t) -1)	Error return.  
 * #define	SIG_DFL	 ((__sighandler_t)  0)	Default action. 
 * #define	SIG_IGN	 ((__sighandler_t)  1)	Ignore signal.
 */

#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define SIG_HOLD ((__sighandler_t)2)
typedef void (*sighandler_t)(int);

sighandler_t
sigset(int sig, sighandler_t disp);
int sighold(int sig);
int sigrelse(int sig);
int sigignore(int sig);
int sigpause(int sig);

sighandler_t
sigset(int sig, sighandler_t disp)
{
    struct sigaction old_set;
    if (disp == SIG_HOLD)
    {
        sighold(sig);

        sigaction(sig, NULL, &old_set);
        return old_set.sa_handler;
    }

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = disp;
    sigaction(sig, &act, &old_set);
    return old_set.sa_handler;
}

int sighold(int sig)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    return sigprocmask(SIG_BLOCK, &set, NULL);
}

int sigrelse(int sig)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);
    return sigprocmask(SIG_UNBLOCK, &set, NULL);
}

int sigignore(int sig)
{
    if (signal(sig, SIG_IGN) == SIG_ERR)
        return -1;
    return 0;
}

int sigpause(int sig)
{
    // 为了原子性，不然在pause()之前就被处理了
    sigset_t old_set;
    if (sigprocmask(SIG_BLOCK, NULL, &old_set) == -1)
        return -1;
    sigdelset(&old_set, sig);
    return sigsuspend(&old_set);
}

void handler(int sig)
{
    printf("handler\n"); // it's unsafe
}

int main()
{
    sigset(SIGTERM, handler);
    sighold(SIGABRT);
    sigrelse(SIGABRT);
    sigignore(SIGABRT);
    sighold(SIGTERM);
    raise(SIGTERM);
    sigpause(SIGTERM);

    return 0;
}
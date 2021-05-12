#include <pthread.h>
#include "tlpi_hdr.h"

struct thread_info
{
    pthread_t thr;
    int loop;
};

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static volatile int glob = 0; /* "volatile" prevents compiler optimizations
                                   of arithmetic operations on 'glob' */
static void *                 /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    struct thread_info *pthr = (struct thread_info *)arg;
    int loops = pthr->loop;
    int j;

    for (j = 0; j < loops; j++)
    {
        pthread_mutex_lock(&mutex);
        if (j % 100 == 0)
            printf("%d %ld\n", glob++, pthr->thr);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    struct thread_info thr1, thr2;
    int s;

    thr1.loop = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;
    thr2.loop = thr1.loop;

    s = pthread_create(&thr1.thr, NULL, threadFunc, (void *)(&thr1));
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&thr2.thr, NULL, threadFunc, (void *)(&thr2));
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(thr1.thr, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(thr2.thr, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}

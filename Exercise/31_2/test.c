#include <pthread.h>
#include <tlpi_hdr.h>
#include "ts_dirname.h"

static char name[1024] = "a/b";
pthread_t thr1, thr2;

void *func(void *arg)
{
    char *p = (char *)arg;

#ifdef THREAD_LOCAL_STORGE
    printf("test for thread_local stroge approach\n");
    printf("%s\n", tls_dirname(p));
#else
    printf("test for thread_specific data approach\n");
    printf("%s\n", ts_dirname(p));
#endif

    return NULL;
}

int main()
{
    int r;
    r = pthread_create(&thr1, NULL, func, (void *)name);
    if (r != 0)
        errExitEN(r, "create");
    r = pthread_create(&thr2, NULL, func, (void *)name);
    if (r != 0)
        errExitEN(r, "create");

    r = pthread_join(thr1, NULL);
    if (r != 0)
        errExitEN(r, "join");
    r = pthread_join(thr2, NULL);
    if (r != 0)
        errExitEN(r, "join");

    return 0;
}
#include <pthread.h>
#include "tlpi_hdr.h"

void *pthread_func(void *arg)
{
    printf("Pthread t1 is running.\n");
}

int main(int argc, char *argv[])
{
    pthread_t t1;
    int s;

    s = pthread_create(&t1, NULL, pthread_func, NULL);
    if (s != 0)
        errExitEN(s, "pthread_create");

    // It occurs a runtime error: ERROR [EDEADLK/EDEADLOCK Resource deadlock avoided] pthread_join
    s = pthread_join(pthread_self(), NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("Pthread t1 is over.\n");

    return 0;
}
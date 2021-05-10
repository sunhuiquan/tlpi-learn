#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <tlpi_hdr.h>
#include <stdlib.h>
#include <unistd.h>

void *thread_func(void *arg)
{
}

int main()
{
    pthread_t thr;
    int s;

    s = pthread_create(&thr, NULL, thread_func, NULL);
    if (s != 0)
    {
        printf("pthread_create failed\n");
        exit(1);
    }

    s = pthread_join(pthread_self(), NULL);
    if (s != 0)
    {
        printf("%s\n", strerror(s));
    }

    return 0;
}
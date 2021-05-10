#include <pthread.h>
#include "tlpi_hdr.h"

void *pthread_func(void *arg)
{
    printf("aaa");
}

int main(int argc, char *argv[])
{
    pthread_t t1;
    int s;

    s = pthread_create(&t1, NULL, pthread_func, NULL);
    if (s != 0)
        printf("pthread_create failed\n");

    s = pthread_join(pthread_self(), NULL);
    if (s != 0)
    {
        printf("%s\n", strerror(s));
        errExitEN(s, "");
    }

    return 0;
}
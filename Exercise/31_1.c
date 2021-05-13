#define _GNU_SOURCE

#include <pthread.h>
#include <tlpi_hdr.h>
#include <stdbool.h>

// int pthread_once(thread_once_t *__once_control, void (*__init_routine)(void))

typedef struct thread_once thread_once;
struct thread_once
{
    bool flag;
    pthread_mutex_t mutex;
};

int one_time_init(thread_once *control, void (*init)(void))
{
    if (control->flag == false) // To improve effective by avoiding useless mutex's lock
        return 0;

    int r = pthread_mutex_lock(&control->mutex);
    if (r != 0)
        return r;

    if (control->flag)
    {
        control->flag = false;
        init();
    }

    r = pthread_mutex_unlock(&control->mutex);
    if (r != 0)
        return r;

    return 0;
}

// static pthread_once_t ponce = PTHREAD_ONCE_INIT;
static thread_once once;
static int a = 0;

void init(void)
{
    ++a;
}

void *func(void *arg)
{
    // pthread_once(&ponce, init);
    one_time_init(&once, init);
    return NULL;
}

int main()
{
    int r;
    pthread_t ths[10];

    once.flag = true;
    r = pthread_mutex_init(&once.mutex, NULL);
    if (r != 0)
        errExitEN(r, "pthread_mutex_init");

    for (int i = 0; i < 10; ++i)
    {
        r = pthread_create(&ths[i], NULL, func, NULL);
        if (r != 0)
            errExitEN(r, "pthread_mutex_init");
    }

    for (int i = 0; i < 10; ++i)
    {
        r = pthread_join(ths[i], NULL);
        if (r != 0)
            errExitEN(r, "pthread_mutex_init");
    }

    printf("%d\n", a);

    return 0;
}

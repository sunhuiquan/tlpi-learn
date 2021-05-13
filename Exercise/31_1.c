/* Solution for Exercise 31-1 */

/* one_time_init.c

   The one_time_init() function implemented here performs the same task as
   the POSIX threads pthread_once() library function.
*/
#include <pthread.h>
#include "tlpi_hdr.h"

struct once_struct
{ /* Our equivalent of pthread_once_t */
    pthread_mutex_t mtx;
    int called;
};

#define ONCE_INITIALIZER             \
    {                                \
        PTHREAD_MUTEX_INITIALIZER, 0 \
    }

struct once_struct once = ONCE_INITIALIZER;

static int
one_time_init(struct once_struct *once_control, void (*init)(void))
{
    int s;

    s = pthread_mutex_lock(&(once_control->mtx));
    if (s == -1)
        errExitEN(s, "pthread_mutex_lock");

    if (!once_control->called)
    {
        (*init)();
        once_control->called = 1;
    }

    s = pthread_mutex_unlock(&(once_control->mtx));
    if (s == -1)
        errExitEN(s, "pthread_mutex_unlock");

    return 0;
}

/* Remaining code is for testing one_time_init() */

static void
init_func()
{
    /* We should see this message only once, no matter how many
       times one_time_init() is called */

    printf("Called init_func()\n");
}

static void *
threadFunc(void *arg)
{
    /* The following allows us to verify that even if a single thread calls
       one_time_init() multiple times, init_func() is called only once */

    one_time_init(&once, init_func);
    one_time_init(&once, init_func);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int s;

    /* Create two threads, both of which will call one_time_init() */

    s = pthread_create(&t1, NULL, threadFunc, (void *)1);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_create(&t2, NULL, threadFunc, (void *)2);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    printf("First thread returned\n");

    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    printf("Second thread returned\n");

    exit(EXIT_SUCCESS);
}

// **************************************

// #include <pthread.h>
// #include <tlpi_hdr.h>
// #include <stdbool.h>

// // int pthread_once(thread_once_t *__once_control, void (*__init_routine)(void))

// typedef struct thread_once thread_once;
// struct thread_once
// {
//     bool flag;
//     pthread_mutex_t mutex;
// };

// int one_time_init(thread_once *control, void (*init)(void))
// {
//     if (control->flag == false) // To improve effective by avoiding useless mutex's lock
//         return 0;

//     int r = pthread_mutex_lock(&control->mutex);
//     if (r != 0)
//         return r;

//     if (control->flag)
//     {
//         control->flag = false;
//         init();
//     }

//     r = pthread_mutex_unlock(&control->mutex);
//     if (r != 0)
//         return r;

//     return 0;
// }

// // static pthread_once_t ponce = PTHREAD_ONCE_INIT;
// static thread_once once;
// static int a = 0;

// void init(void)
// {
//     ++a;
// }

// void *func(void *arg)
// {
//     // pthread_once(&ponce, init);
//     one_time_init(&once, init);
//     return NULL;
// }

// int main()
// {
//     int r;
//     pthread_t ths[10];

//     once.flag = true;
//     r = pthread_mutex_init(&once.mutex, NULL);
//     if (r != 0)
//         errExitEN(r, "pthread_mutex_init");

//     for (int i = 0; i < 10; ++i)
//     {
//         r = pthread_create(&ths[i], NULL, func, NULL);
//         if (r != 0)
//             errExitEN(r, "pthread_mutex_init");
//     }

//     for (int i = 0; i < 10; ++i)
//     {
//         r = pthread_join(ths[i], NULL);
//         if (r != 0)
//             errExitEN(r, "pthread_mutex_init");
//     }

//     printf("%d\n", a);

//     return 0;
// }

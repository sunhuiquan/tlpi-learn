#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include <tlpi_hdr.h>

#define MAX_THREAD_NUM 20
#define LOOPS 100000

static volatile long glob = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg)
{
    for (int i = 0; i < LOOPS; ++i)
    {
        pthread_mutex_lock(&mutex);
        glob++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thrs[MAX_THREAD_NUM];
    int pthread_nums;

    if (argc != 2 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
    {
        printf("%s usage: thread-nums\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    pthread_nums = atoi(argv[1]);
    if (pthread_nums > MAX_THREAD_NUM)
    {
        printf("too many treads\n");
        exit(EXIT_SUCCESS);
    }

    struct timeval start_time, end_time;
    double time_use;
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < pthread_nums; ++i)
        pthread_create(&thrs[i], NULL, thread_func, NULL);

    for (int i = 0; i < pthread_nums; ++i)
        pthread_join(thrs[i], NULL);

    gettimeofday(&end_time, NULL);
    time_use = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    printf("mutex costs %.3lf secs\n", time_use);

    assert(glob == LOOPS * pthread_nums);
    return 0;
}
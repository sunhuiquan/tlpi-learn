#include <pthread.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <assert.h>
#include <tlpi_hdr.h>

#define MAX_THREAD_NUM 20
#define LOOPS 100000

static volatile int semid;
static volatile long glob = 0;
struct sembuf sops[2]; // 0 is wait and 1 is post

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

void *
thread_func(void *arg)
{
    for (int i = 0; i < LOOPS; ++i)
    {
        if (semop(semid, &sops[0], 1) == -1)
            errExit("semop-wait");
        glob++;
        if (semop(semid, &sops[1], 1) == -1)
            errExit("semop-post");
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thrs[MAX_THREAD_NUM];
    int pthread_nums;
    union semun arg;

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

    if ((semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1)
        errExit("semget");

    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1)
        errExit("semctl-SETVAL");

    sops[0].sem_flg = 0;
    sops[0].sem_num = 0;
    sops[0].sem_op = -1;

    sops[1].sem_flg = 0;
    sops[1].sem_num = 0;
    sops[1].sem_op = 1;

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

    if (semctl(semid, 0, IPC_RMID) == -1)
        errExit("semctl-IPC_RMID");

    assert(glob == LOOPS * pthread_nums);
    return 0;
}
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <tlpi_hdr.h>

#define WR_SEM "/write_sem_53_1"
#define RD_SEM "/read_sem_53_1"
#define BUFFER_SIZE 4096

static char buffer[BUFFER_SIZE];
static volatile ssize_t num;
static sem_t *write_sem;
static sem_t *read_sem;

void *read_func(void *arg)
{
    int xfrs, bytes;

    for (xfrs = 0, bytes = 0;; xfrs++)
    {
        if (sem_wait(read_sem) == -1)
            errExit("sem_wait");

        if (num == 0) /* Writer encountered EOF */
            break;
        bytes += num;

        if (write(STDOUT_FILENO, buffer, num) != num)
            errExit("write");

        if (sem_post(write_sem) == -1)
            errExit("sem_post");
    }

    if (sem_wait(read_sem) == -1)
        errExit("sem_wait");

    printf("read %d bytes(%d)", bytes, xfrs);

    return NULL;
}

void *write_func(void *arg)
{
    int xfrs, bytes;

    for (xfrs = 0, bytes = 0;; xfrs++, bytes += num)
    {
        if (sem_wait(write_sem) == -1)
            errExit("sem_wait");

        num = read(STDIN_FILENO, buffer, BUFFER_SIZE);
        if (num == -1)
            errExit("read");

        if (sem_post(read_sem) == -1)
            errExit("sem_post");

        if (num == 0)
            break;
    }

    printf("write %d bytes(%d)", bytes, xfrs);

    if (sem_post(read_sem) == -1)
        errExit("sem_post");

    return NULL;
}

int main()
{
    pthread_t r_thr, w_thr;

    write_sem = sem_open(WR_SEM, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if (write_sem == SEM_FAILED)
        errExit("sem_open");
    read_sem = sem_open(RD_SEM, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    if (write_sem == SEM_FAILED)
        errExit("sem_open");

    if (pthread_create(&r_thr, NULL, read_func, NULL) == -1)
        errExit("pthread_create");
    if (pthread_create(&w_thr, NULL, write_func, NULL) == -1)
        errExit("pthread_create");

    if (pthread_join(r_thr, NULL) == -1)
        errExit("pthread_join");
    if (pthread_join(w_thr, NULL) == -1)
        errExit("pthread_join");

    if (sem_unlink(WR_SEM) == -1)
        errExit("sem_unlink");
    if (sem_unlink(RD_SEM) == -1)
        errExit("sem_unlink");

    return 0;
}
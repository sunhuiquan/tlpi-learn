#include <semaphore.h>
#include <time.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
    sem_t *sem;
    struct timespec timeout;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sem-name secs\n", argv[0]);

    sem = sem_open(argv[1], 0);
    if (sem == SEM_FAILED)
        errExit("sem_open");

    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += atoi(argv[2]);
    if (sem_timedwait(sem, &timeout) == -1)
        errExit("sem_timewait");

    printf("%ld sem_wait() succeeded\n", (long)getpid());
    exit(EXIT_SUCCESS);
}

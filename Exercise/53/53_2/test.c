#include <tlpi_hdr.h>
#include <semaphore.h>
#include <fcntl.h>

int main()
{
    sem_t *sem;

    sem = sem_open("/aaa", O_CREAT | O_EXCL | O_RDWR, 0666, 1); // 对于信号量默认是读写不是只读
    if (sem == SEM_FAILED)
        errExit("sem_open");

    return 0;
}
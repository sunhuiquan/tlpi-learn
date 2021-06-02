#include <sys/sem.h>
#include "semun.h"
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    union semun arg;
    int maxind, ind, semid;
    struct semid_ds ds;
    struct seminfo seminfo;

    /* Obtain size of kernel 'entries' array */
    arg.__buf = &seminfo;
    maxind = semctl(0, 0, SEM_INFO, arg);
    if (maxind == -1)
        errExit("msgctl-SEM_INFO");

    printf("maxind: %d\n\n", maxind);
    printf("index     id       key      \n");

    /* Retrieve and display information from each element of 'entries' array */
    arg.buf = &ds;
    for (ind = 0; ind <= maxind; ind++)
    {
        semid = semctl(ind, 0, SEM_STAT, arg);
        if (semid == -1)
        {
            if (errno != EINVAL && errno != EACCES)
                errMsg("msgctl-SEM_STAT"); /* Unexpected error */
            continue;                      /* Ignore this item */
        }

        printf("%4d %8d  0x%08lx\n", ind, semid,
               (unsigned long)ds.sem_perm.__key);
    }

    exit(EXIT_SUCCESS);
}

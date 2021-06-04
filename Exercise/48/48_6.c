#include <sys/shm.h>
#include <sys/stat.h>
#include <tlpi_hdr.h>

int main()
{
    struct shminfo info;
    int maxind, ind, shmid;
    struct shmid_ds ds;

    if ((maxind = shmctl(0, SHM_INFO, (struct shmid_ds *)&info)) == -1)
        errExit("shmctl");

    printf("maxind: %d\n\n", maxind);
    printf("index     id       key      nattch      \n");

    for (ind = 0; ind <= maxind; ++ind)
    {
        if ((shmid = shmctl(ind, SHM_STAT, &ds)) == -1)
        {
            if (errno != EINVAL && errno != EACCES)
                errExit("shmctl");
            continue;
        }

        printf("%4d %8d 0x%08lx %04d\n", ind, shmid, (long)ds.shm_perm.__key, (int)ds.shm_nattch);
    }

    return 0;
}
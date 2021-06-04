#include <tlpi_hdr.h>
#include <sys/shm.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int shmid;
    struct shmid_ds ds;

    if (argc != 2)
    {
        printf("%s usage: shmid\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    shmid = atoi(argv[1]);

    if (shmctl(shmid, IPC_STAT, &ds) == -1)
        errExit("shmctl - IPC_STAT");

    printf("key: %lx\nshmid: %d\nshm segsz: %ld\nshm nattch: %ld\n",
           (long)ds.shm_perm.__key, shmid, (long)ds.shm_segsz, (long)ds.shm_nattch);

    return 0;
}
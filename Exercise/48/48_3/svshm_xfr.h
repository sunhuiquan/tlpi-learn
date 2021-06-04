#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "../../../tlpi-dist/svsem/binary_sems.h" /* Declares our binary semaphore functions */
#include "tlpi_hdr.h"

/* Hard-coded keys for IPC objects */

#define SHM_KEY 0x1234 /* Key for shared memory segment */
#define SEM_KEY 0x5678 /* Key for semaphore set */

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
/* Permissions for our IPC objects */

/* Two semaphores are used to ensure exclusive, alternating access
   to the shared memory segment */

#define WRITE_SEM 0 /* Writer has access to shared memory */
#define READ_SEM 1  /* Reader has access to shared memory */

#ifndef BUF_SIZE      /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024 /* Size of transfer buffer */
#endif

struct shmseg
{                       /* Defines structure of shared memory segment */
    int cnt;            /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE]; /* Data being transferred */
};

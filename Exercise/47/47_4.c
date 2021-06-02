#include <sys/stat.h>
#include <sys/sem.h>
#include <string.h>
#include "tlpi_hdr.h"
#include "semun.h"

int main(int argc, char *argv[])
{
    int semid = atoi(argv[1]);

    printf("SEMPID: %d\n", semctl(semid, 0, GETVAL));
    return 0;
}

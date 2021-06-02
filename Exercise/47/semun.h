#ifndef SEMUN_H
#define SEMUN_H

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

#endif
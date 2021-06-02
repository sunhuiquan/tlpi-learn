#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <tlpi_hdr.h>

int main()
{
    int maxind, ind, msqid;
    struct msqid_ds ds;
    struct msginfo msginfo;

    maxind = msgctl(0, MSG_INFO, (struct msqid_ds *)&msginfo);
    if (maxind == -1)
        errExit("MSG_INFO");

    for (ind = 0; ind <= maxind; ++ind)
    {
        msqid = msgctl(ind, MSG_STAT, &ds);
        if (msqid == -1)
        {
            if (errno != EINVAL && errno != EACCES)
                errExit("MSG_STAT");
            continue;
        }
        /* id = indexx + xxx_perm.__seq * SEQ_MULTIPLIER */
        printf("msqid: %d\n", msqid);
        printf("index: %d\nseq: %ld\nSEQ_MULTIPLIER: 32768\ncalculate: %ld\n",
               ind, (long)ds.msg_perm.__seq, ind + (long)ds.msg_perm.__seq * 32768);
        printf("------------------------\n\n");
    }

    return 0;
}
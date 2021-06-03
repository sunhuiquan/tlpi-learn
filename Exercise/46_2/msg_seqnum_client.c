#include <tlpi_hdr.h>
#include <sys/msg.h>
#include <fcntl.h>
#include "msg_seqnum.h"

int main(int argc, char *argv[])
{
    int client_msqid, fd;
    struct RequestMsg request_msg;
    struct ResponseMsg response_msg;

    if (argc != 2)
    {
        printf("usage: %s seqlen\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    if ((fd = open(SERVER_FILE, O_RDONLY)) == -1)
        errExit("open");
    if (read(fd, &client_msqid, sizeof(client_msqid)) != sizeof(client_msqid))
        errExit("read");
    if (close(fd) == -1)
        errExit("close");

    request_msg.mtype = 1;
    request_msg.pid = getpid();
    request_msg.seqlen = atoi(argv[1]);
    if (msgsnd(client_msqid, &request_msg, REQUSET_SIZE, 0) == -1)
        errExit("msgsnd");

    if (msgrcv(client_msqid, &response_msg, RESPONSE_SIZE, getpid(), 0) == -1)
        errExit("msgrcv");

    printf("%d\n", response_msg.seqnum);

    return 0;
}
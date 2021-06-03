#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tlpi_hdr.h>
#include "msg_seqnum.h"

#define MSG_MODE ((S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP))

int main()
{
    int server_msqid, fd;
    struct RequestMsg request_msg;
    struct ResponseMsg response_msg;
    int seq_num = 0;

    if ((server_msqid = msgget(IPC_PRIVATE, MSG_MODE)) == -1)
        errExit("msgget");
    if ((fd = open(SERVER_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0666)) == -1)
        errExit("open");
    if (write(fd, &server_msqid, sizeof(server_msqid)) != sizeof(server_msqid))
        errExit("write");
    if (close(fd) == -1)
        errExit("close");

    for (;;)
    {
        if (msgrcv(server_msqid, &request_msg, REQUSET_SIZE, 1, 0) == -1)
            errExit("msgrcv");

        response_msg.mtype = (long)request_msg.pid;
        response_msg.seqnum = seq_num;
        // we should send the actual bytes we use in msgsnd
        if (msgsnd(server_msqid, &response_msg, RESPONSE_SIZE, 0) == -1)
            errExit("msgsnd");

        seq_num += request_msg.seqlen;
    }

    exit(EXIT_FAILURE);
}
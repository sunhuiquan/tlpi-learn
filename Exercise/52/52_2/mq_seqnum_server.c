#include <signal.h>
#include <mqueue.h>
#include <tlpi_hdr.h>
#include "mq_seqnum.h"

int main(int argc, char *argv[])
{
    mqd_t serverFd, clientFd;
    struct mq_attr attr;
    char clientMQ[CLIENT_MQ_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0; /* This is our "service" */

    attr.mq_flags = 0;            // 虽然在mq_open上这个参数会被忽略无所谓值是什么，只是为了好看
    attr.mq_maxmsg = sizeof(req); // 最大req够用(req的大小比resp大，这两种消息都够传输了)
    attr.mq_maxmsg = 10;
    umask(0); /* So we get the permissions we want */
    if ((serverFd = mq_open(MQ_SERVER, O_CREAT | O_EXCL | O_RDONLY, 0666, &attr)) == -1)
        errExit("mq_open");

    for (;;)
    {
        // 这种传输的大小一定是这么大
        if (mq_receive(serverFd, &req, sizeof(req), NULL) != sizeof(req))
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue; /* Either partial read or error */
        }

        snprintf(clientMQ, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
                 (long)req.pid);
        if ((clientFd = mq_open(clientMQ, O_WRONLY, 0, &attr)) == -1)
        { /* Open failed, give up on client */
            errMsg("open %s", clientMQ);
            continue;
        }

        resp.seqNum = seqNum;
        if (mq_send(clientFd, &resp, sizeof(struct response), NULL) == -1)
            fprintf(stderr, "Error writing to FIFO %s\n", clientMQ);
        if (mq_close(clientFd) == -1)
            errMsg("close");

        seqNum += req.seqLen; /* Update our sequence number */
    }
}

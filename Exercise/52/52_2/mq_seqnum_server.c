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

    attr.mq_flags = 0; // 虽然在mq_open上这个参数会被忽略无所谓值是什么，只是为了好看
    attr.mq_msgsize = sizeof(req);
    attr.mq_maxmsg = 10;
    if ((serverFd = mq_open(MQ_SERVER, O_CREAT | O_EXCL | O_RDONLY, 0666, &attr)) == -1)
        errExit("mq_open");

    for (;;)
    {
        // 这种传输的大小一定是这么大
        if (mq_receive(serverFd, (char *)&req, attr.mq_msgsize, NULL) != sizeof(req))
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue; /* Either partial read or error */
        }

        snprintf(clientMQ, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
                 (long)req.pid);
        if ((clientFd = mq_open(clientMQ, O_WRONLY, 0, &attr)) == -1)
        { /* Open failed, give up on client */
            errMsg("open");
            continue;
        }

        resp.seqNum = seqNum;
        if (mq_send(clientFd, (char *)&resp, sizeof(struct response), 0) == -1)
            fprintf(stderr, "Error writing to FIFO %s\n", clientMQ);
        if (mq_close(clientFd) == -1)
            errMsg("close");

        seqNum += req.seqLen; /* Update our sequence number */
    }
}

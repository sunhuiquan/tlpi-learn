#include <mqueue.h>
#include <tlpi_hdr.h>
#include "mq_seqnum.h"

static char clientMQ[CLIENT_MQ_NAME_LEN];

static void /* Invoked on exit to delete client FIFO */
removeFifo(void)
{
    mq_unlink(clientMQ);
}

int main(int argc, char *argv[])
{
    mqd_t serverFd, clientFd;
    struct request req;
    struct response resp;
    struct mq_attr attr;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [seq-len]\n", argv[0]);

    /* Create our FIFO (before sending request, to avoid a race) */

    snprintf(clientMQ, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
             (long)getpid());

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(req);
    if ((clientFd = mq_open(clientMQ, O_CREAT | O_EXCL | O_RDONLY, 0666, &attr)) == -1)
        errExit("mq_open");

    if (atexit(removeFifo) != 0)
        errExit("atexit");

    req.pid = getpid();
    req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

    serverFd = mq_open(MQ_SERVER, O_WRONLY);
    if (serverFd == -1)
        errExit("mq_open");

    if (mq_send(serverFd, (char *)&req, sizeof(req), 0) == -1)
        fatal("Can't write to server");

    /* Open our FIFO, read and display response */

    clientFd = mq_open(clientMQ, O_RDONLY);
    if (clientFd == -1)
        errExit("mq_open");

    if (mq_receive(clientFd, (char *)&resp, attr.mq_msgsize, NULL) != sizeof(struct response))
        fatal("Can't read response from server");

    printf("%d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}

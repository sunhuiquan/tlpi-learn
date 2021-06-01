#include <signal.h>
#include "fifo_seqnum.h"

int main(int argc, char *argv[])
{
    int serverFd, dummyFd, clientFd, save_fd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum; /* This is our "service" */

    if ((save_fd = open(SAVE_FILE_PATH, O_RDWR | O_SYNC)) == -1)
    {
        if (errno == ENOENT)
        {
            save_fd = open(SAVE_FILE_PATH, O_CREAT | O_RDWR | O_SYNC, 0666);
            if (save_fd == -1)
                errExit("open");
            seqNum = 0;
            if (write(save_fd, &seqNum, sizeof(seqNum)) != sizeof(seqNum))
                errExit("write");
            lseek(save_fd, 0, SEEK_SET);
        }
        else
            errExit("open");
    }
    else if (read(save_fd, &seqNum, sizeof(seqNum)) != sizeof(seqNum))
        errExit("read");

    /* Create well-known FIFO, and open it for reading */
    umask(0); /* So we get the permissions we want */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);
    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);

    /* Open an extra write descriptor, so that we never see EOF */

    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1)
        errExit("open %s", SERVER_FIFO);

    /* Let's find out about broken client pipe via failed write() */

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");

    for (;;)
    { /* Read requests and send responses */
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue; /* Either partial read or error */
        }

        /* Open client FIFO (previously created by client) */

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                 (long)req.pid);
        clientFd = open(clientFifo, O_WRONLY);
        if (clientFd == -1)
        { /* Open failed, give up on client */
            errMsg("open %s", clientFifo);
            continue;
        }

        /* Send response and close FIFO */

        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        if (close(clientFd) == -1)
            errMsg("close");

        seqNum += req.seqLen; /* Update our sequence number */
        if (write(save_fd, &seqNum, sizeof(seqNum)) != sizeof(seqNum))
            errExit("write");
        lseek(save_fd, 0, SEEK_SET);
    }
}

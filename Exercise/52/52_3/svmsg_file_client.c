#include <mqueue.h>
#include "svmsg_file.h"

char client_name[PATH_MAX];

static void
removeQueue(void)
{
    mq_unlink(client_name);
}

int main(int argc, char *argv[])
{
    struct requestMsg req;
    struct responseMsg resp;
    int numMsgs;
    mqd_t server_mqd, client_mqd;
    struct mq_attr attr;
    ssize_t msgLen, totBytes;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    if (strlen(argv[1]) > sizeof(req.pathname) - 1)
        cmdLineErr("pathname too long (max: %ld bytes)\n",
                   (long)sizeof(req.pathname) - 1);

    if ((server_mqd = mq_open(SERVER_FILE, O_WRONLY)) == -1)
        errExit("mq_open");

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct responseMsg); // it's bigger than struct response
    // 如果超过linux的限制(在相应proc目录下的文件里写着，需要手动修改，否则是EINVAL错误)
    // 注意sudo的时候没有错误，因为拥有特权不受那个的软限制
    sprintf(client_name, CLIENT_FILE, (long)getpid());
    if ((client_mqd = mq_open(client_name, O_CREAT | O_EXCL | O_RDONLY, 0666, &attr)) == -1)
        errExit("mq_open");

    if (atexit(removeQueue) != 0)
        errExit("atexit");

    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';
    req.pid = getpid();
    if (mq_send(server_mqd, (char *)&req, sizeof(req.pid) + strlen(req.pathname) + 1, 0) == -1)
        errExit("mq_send");

    if ((msgLen = mq_receive(client_mqd, (char *)&resp, attr.mq_msgsize, NULL)) == -1)
        errExit("mq_receive");

    if (resp.type == RESP_MT_FAILURE)
    {
        printf("%s\n", resp.data); /* Display msg from server */
        exit(EXIT_FAILURE);
    }

    /* File was opened successfully by server; process messages
       (including the one already received) containing file data */

    totBytes = msgLen - sizeof(mtype); /* Count first message */
    for (numMsgs = 1; resp.type == RESP_MT_DATA; numMsgs++)
    {
        if ((msgLen = mq_receive(client_mqd, (char *)&resp, attr.mq_msgsize, NULL)) == -1)
            errExit("mq_receive");

        totBytes += msgLen - sizeof(mtype);
    }

    printf("Received %ld bytes (%d messages)\n", (long)totBytes, numMsgs);
    /*FIXME: above: should use %zd here, and remove (long) cast (or perhaps
       better, make totBytes size_t, and use %zu)*/

    exit(EXIT_SUCCESS);
}

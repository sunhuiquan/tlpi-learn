#include <mqueue.h>
#include "svmsg_file.h"

static void /* SIGCHLD handler */
grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno; /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    // 用while是为了保证回收所有的僵尸子进程，因为可能在阻塞信号的过程中出现丢失
    errno = savedErrno;
}

static void /* Executed in child process: serve a single client */
serveRequest(const struct requestMsg *req)
{
    int fd;
    ssize_t numRead;
    struct responseMsg resp;
    mqd_t client_id;
    char client_name[PATH_MAX];

    sprintf(client_name, CLIENT_FILE, (long)req->pid);
    if ((client_id = mq_open(client_name, O_WRONLY)) == -1)
        errExit("mq_open");

    fd = open(req->pathname, O_RDONLY);
    if (fd == -1)
    { /* Open failed: send error text */
        resp.type = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");

        // 切记这种手动计算要发送的字节数的情况，千万别忘了字节对齐的计算，这里是后面是char类型不存在
        // 字节对齐的问题，否则要用 offsetof 计算含着填充字节的真实的要传输的大小
        if (mq_send(client_id, (char *)&resp, sizeof(resp.type) + strlen(resp.data) + 1, 0) == -1)
            errExit("mq_send");
        exit(EXIT_FAILURE); /* and terminate */
    }

    resp.type = RESP_MT_DATA;
    while ((numRead = read(fd, resp.data, DATA_MSG_SIZE)) > 0)
        if (mq_send(client_id, (char *)&resp, sizeof(resp.type) + numRead, 0) == -1)
            break;

    resp.type = RESP_MT_END;
    if (mq_send(client_id, (char *)&resp, sizeof(resp.type) + 0, 0) == -1)
        errExit("mq_send");
}

int main(int argc, char *argv[])
{
    struct requestMsg req;
    pid_t pid;
    ssize_t msgLen;
    mqd_t server_id;
    struct sigaction sa;
    struct mq_attr attr;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct requestMsg);
    if ((server_id = mq_open(SERVER_FILE, O_CREAT | O_EXCL | O_RDONLY, 0666, &attr)) == -1)
        errExit("mq_open");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    /* Read requests, handle each in a separate child process */

    for (;;)
    {
        msgLen = mq_receive(server_id, (char *)&req, attr.mq_msgsize, NULL);
        if (msgLen == -1)
        {
            if (errno == EINTR)
                continue;
            errExit("mq_receive");
        }

        pid = fork(); /* Create child process */
        if (pid == -1)
        {
            errMsg("fork");
            break;
        }

        if (pid == 0)
        {
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }
    }

    if (mq_unlink(SERVER_FILE) == -1)
        errExit("mq_unlink");
    exit(EXIT_SUCCESS);
}

#include <mqueue.h>
#include <tlpi_hdr.h>

int main()
{
    int mqd;

    mqd = mq_open("/aaa", O_CREAT | O_EXCL | O_RDWR, 0666, NULL);
    if (mqd == (mqd_t)-1)
        errExit("mq_open");

    sleep(20);

    if (mq_send(mqd, "aaa", 3, 0) == -1)
        errExit("mq_send");

    mq_close(mqd);
    mq_unlink("/aaa");
    return 0;
}
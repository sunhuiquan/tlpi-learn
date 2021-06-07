#include <mqueue.h>
#include <tlpi_hdr.h>
int main()
{
    mqd_t mqd;
    if ((mqd = mq_open("/a", O_CREAT | O_EXCL | O_WRONLY, 0666, NULL)) == -1)
        errExit("mq_open");

    sleep(10);
    printf("a\n");

    for (;;)
    {
        sleep(3);
        if (mq_send(mqd, "a", 1, 0) == -1)
            errExit("mq_send");
    }

    return 0;
}
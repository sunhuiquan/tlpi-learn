#include <mqueue.h>
#include <fcntl.h> /* For definition of O_NONBLOCK */
#include <time.h>
#include "tlpi_hdr.h"

static void
usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] mq-name secs\n", progName);
    fprintf(stderr, "    -n           Use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t numRead;
    struct timespec timeout;

    flags = O_RDONLY;
    while ((opt = getopt(argc, argv, "n")) != -1)
    {
        switch (opt)
        {
        case 'n':
            flags |= O_NONBLOCK;
            break;
        default:
            usageError(argv[0]);
        }
    }

    if (optind + 1 >= argc)
        usageError(argv[0]);

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t)-1)
        errExit("mq_open");

    /* We need to know the 'mq_msgsize' attribute of the queue in
       order to determine the size of the buffer for mq_receive() */

    if (mq_getattr(mqd, &attr) == -1)
        errExit("mq_getattr");

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        errExit("malloc");

    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += atoi(argv[optind + 1]);

    numRead = mq_timedreceive(mqd, buffer, attr.mq_msgsize, &prio, &timeout);
    if (numRead == -1)
    {
        if (errno == ETIMEDOUT)
        {
            printf("receive time out\n");
            exit(EXIT_SUCCESS);
        }
        else
            errExit("mq_receive");
    }

    printf("Read %ld bytes; priority = %u\n", (long)numRead, prio);
    /*FIXME: above: should use %zd here, and remove (long) cast */
    if (write(STDOUT_FILENO, buffer, numRead) == -1)
        errExit("write");
    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}

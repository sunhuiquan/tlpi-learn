#ifndef MQ_SEQNUM_H
#define MQ_SEQNUM_H

#define MQ_SERVER "/server_seqnum"
/* Well-known name for server's POSIX message queue */
#define CLIENT_MQ_TEMPLATE "/client_seqnum.%ld"
/* Template for building client POSIX name */
#define CLIENT_MQ_NAME_LEN (sizeof(CLIENT_MQ_TEMPLATE) + 20)
/* Space required for client POSIX mq pathname
                                  (+20 as a generous allowance for the PID) */

struct request
{               /* Request (client --> server) */
    pid_t pid;  /* PID of client */
    int seqLen; /* Length of desired sequence */
};

struct response
{               /* Response (server --> client) */
    int seqNum; /* Start of sequence */
};

#endif

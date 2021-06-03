#ifndef MSG_SEQNUM_H
#define MSG_SEQNUM_H

#include <unistd.h>
#include <stddef.h>

#define SERVER_FILE "/tmp/server_file_msg_seqnum_h"

// note that this struct has padding bytes, using offsetof to deal with it
struct RequestMsg
{
    long mtype;
    pid_t pid;
    int seqlen;
};

struct ResponseMsg
{
    long mtype;
    int seqnum;
};

#define REQUSET_SIZE (offsetof(struct RequestMsg, seqlen) - \
                      offsetof(struct RequestMsg, pid) + sizeof(int))
#define RESPONSE_SIZE (sizeof(int))

#endif

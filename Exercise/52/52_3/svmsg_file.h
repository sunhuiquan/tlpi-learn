#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h> /* For definition of offsetof() */
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

#define SERVER_FILE "/server_file"
#define CLIENT_FILE "/client_file.%ld"

#define DATA_MSG_SIZE 4096

struct requestMsg
{
    pid_t pid;
    char pathname[PATH_MAX];
};

typedef enum mtype mtype;
enum mtype
{
    RESP_MT_END,
    RESP_MT_DATA,
    RESP_MT_FAILURE
};
struct responseMsg
{ /* Responses (server to client) */
    mtype type;
    char data[DATA_MSG_SIZE]; /* File content / response message */
};

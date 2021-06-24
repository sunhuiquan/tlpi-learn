#include <sys/un.h>
#include <sys/socket.h>
#include <tlpi_hdr.h>

#define SOCKET_PATH "mysocket"

struct request
{				/* Request (client --> server) */
	int seqLen; /* Length of desired sequence */
};

struct response
{				/* Response (server --> client) */
	int seqNum; /* Start of sequence */
};

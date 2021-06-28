#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <tlpi_hdr.h>
#include "../../../tlpi-dist/lib/inet_sockets.h"
#include "../../../tlpi-dist/sockets/read_line.h" /* Declaration of readLine() */

#define PORT_NUM "50000" /* Port number for server */

#define INT_LEN 30 /* Size of string able to hold largest \
					  integer (including terminating '\n') */

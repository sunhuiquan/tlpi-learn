#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <tlpi_hdr.h>
#include "my_read_line_buf.h"

#define PORT_NUM "50000" /* Port number for server */

#define INT_LEN 30 /* Size of string able to hold largest \
					  integer (including terminating '\n') */

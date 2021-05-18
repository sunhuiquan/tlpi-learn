#include <unistd.h>
#include "tlpi_hdr.h"

int main()
{
    execl("longest_line.awk", "longest_line.awk", "input.txt", (char *)NULL);

    printf("%s\n", strerror(errno));

    return 0;
}
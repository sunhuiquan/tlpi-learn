#include <unistd.h>
#include <stdlib.h>

int main()
{
    execl("27_3.sh", "27_3.sh", NULL);
    exit(127);
}
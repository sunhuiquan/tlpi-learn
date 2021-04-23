#include "tlpi_hdr.h"
#include <pwd.h>

int main()
{
    struct passwd *pa = getpwnam("root");
    struct passwd *pb = getpwnam("water");
    printf("%u\n", pa->pw_uid);
    printf("%u\n", pb->pw_uid);
}
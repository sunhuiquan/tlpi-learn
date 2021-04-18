// It works fine
// #include <unistd.h>
// #include <sys/reboot.h>

// int main()
// {
//     reboot(RB_AUTOBOOT);
//     return 0;
// }

// It works well too.
#include <unistd.h>
#include <linux/reboot.h>

int reboot(int);
int main()
{
    reboot(LINUX_REBOOT_CMD_RESTART);
    return 0;
}
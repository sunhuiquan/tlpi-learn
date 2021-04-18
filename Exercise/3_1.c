#define _POSIX_SOURCE
#include <unistd.h>
#include <linux/reboot.h>

int main()
{

    reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2A, LINUX_REBOOT_CMD_RESTART, 0);
    return 0;
}
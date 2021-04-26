#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>

#define MAXNAME 256

void print_proc(dirname, uid);

int main(int argc, char *argv[])
{
    char dirname[MAXNAME];
    struct passwd *passwd;
    uid_t uid;
    DIR *dirp;
    struct dirent *dir_entry;
    if (argc != 2)
    {
        printf("usage: <user-name>\n");
        exit(0);
    }

    passwd = getpwnam(argv[1]);
    if (passwd == NULL)
    {
        printf("getpwnam failed\n");
        exit(1);
    }
    uid = passwd->pw_uid;

    dirp = opendir("/proc");
    if (dirp == NULL)
    {
        printf("opendir failed\n");
        exit(1);
    }

    while ((dir_entry = readdir(dirp)) != NULL)
    {
        strcpy(dirname, "/proc/");
        strcat(dirname, dir_entry->d_name);
        print_proc(dirname, uid);
    }
    if (errno != 0)
    {
        printf("readdir failed\n");
        exit(1);
    }

    return 0;
}
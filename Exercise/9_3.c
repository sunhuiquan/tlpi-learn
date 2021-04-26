#include <stdio.h>
#include <grp.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#define MAXLINE 4096

int my_initgroups(const char *user, gid_t group);

int main()
{
    gid_t list[NGROUPS_MAX + 1];
    int gid_size = getgroups(NGROUPS_MAX + 1, list);
    for (int i = 0; i < gid_size; ++i)
        printf("%u  \n", list[i]);
    printf("============================\n");

    my_initgroups("water", 1000);

    gid_size = getgroups(NGROUPS_MAX + 1, list);
    for (int i = 0; i < gid_size; ++i)
        printf("%u  \n", list[i]);

    return 0;
}

int my_initgroups(const char *user, gid_t group)
{
    FILE *fp;
    char line[MAXLINE];
    int n_group = 0;
    gid_t grouplist[NGROUPS_MAX + 1]; // plus one for formal GID

    if ((fp = fopen("/etc/group", "r")) == NULL)
        return -1;

    while (fgets(line, MAXLINE, fp))
    {
        char *p1, *p2;
        p1 = strchr(line, ':');
        *p1 = '\0';
        p1++;
        if (strcmp(line, user) == 0)
        {
            p1 = strchr(p1, ':');
            p1++;
            p1 = strchr(p1, ':');
            p1++;
            if (*p1 != '\0')
                while (p1)
                {
                    p2 = p1;
                    p1 = strchr(p1, ',');
                    if (p1)
                    {
                        *p1 = '\0';
                        p1++;
                    }
                    grouplist[n_group++] = atoi(p2);
                }
            break;
        }
    }
    grouplist[n_group++] = group;
    setgroups(n_group, grouplist);

    // setgroups是暂时的修改
    // system("cat /etc/group");
    // 暂时的修改，只是写到进程的数据结构里头，没有写道
    // group文件里面，因为写入进去就是永久的了

    return 0;
}
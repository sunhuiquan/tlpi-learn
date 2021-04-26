#include <stdio.h>
#include <grp.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <pwd.h>

#define MAXLINE 4096

int my_initgroups(const char *user, gid_t group);

/**
 * set-user-root-ID程序，进入后是r是实际调用的，e和s都是root，
 * my_initgroups指定的用户名找到对应id，设置成进程的实际ID
 */

int main()
{
    /**
     * get/setgroups对应的是进程的实际用户id(即调用的所属的ID)，
     * 通过set-user-root-ID测试。
     * 
     * 要注意的是凭证看有效，但调用一般看所属，所谓的属主指的是实际ID 
     */
    gid_t list[NGROUPS_MAX + 1];
    int gid_size;

    printf("now real user ID: %d\n", getuid());
    gid_size = getgroups(NGROUPS_MAX + 1, list);
    for (int i = 0; i < gid_size; ++i)
        printf("%u  \n", list[i]);

    // 永远要做错误检查，节约debug的时间太多了
    if (my_initgroups("water", 1000) < 0)
        printf("my_initgroups error: %s\n", strerror(errno));

    printf("now real user ID: %d\n", getuid());
    gid_size = getgroups(NGROUPS_MAX + 1, list);
    for (int i = 0; i < gid_size; ++i)
        printf("%u  \n", list[i]);

    return 0;
}

int my_initgroups(const char *user, gid_t group)
{
    struct passwd *passwd;
    int real_id;
    FILE *fp;
    char line[MAXLINE];
    int n_group = 0;
    gid_t grouplist[NGROUPS_MAX + 1]; // plus one for formal GID

    passwd = getpwnam(user);
    if (passwd == NULL)
        return -1;

    real_id = passwd->pw_uid;
    setreuid(real_id, -1);
    /**
     *  r        e    s
     *  real_id  0    0
     */
    if ((fp = fopen("/etc/group", "r")) == NULL) // e_uid是root权限没问题
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
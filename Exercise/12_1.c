#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <pwd.h>
#include <stdlib.h>

#define MAXNAME 256
#define MAXLINE 4096

void trim(char *s);
int print_proc(const char *dir, uid_t uid);

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
        if (dir_entry->d_type != DT_DIR)
            continue;
        strcpy(dirname, "/proc/");
        strcat(dirname, dir_entry->d_name);
        if (print_proc(dirname, uid) < 0)
        {
            printf("print_proc failed\n");
            exit(1);
        }
    }

    return 0;
}

void trim(char *s)
{
    char *p = s;
    int l = strlen(p);

    while (isspace(p[l - 1]))
        p[--l] = 0;
    while (*p && isspace(*p))
        ++p, --l;

    memmove(s, p, l + 1);
}

int print_proc(const char *dir, uid_t uid)
{
    char filename[MAXNAME];
    char proc_name[MAXLINE];
    char str[MAXLINE];
    char pid[MAXLINE];
    char uid_str[MAXLINE];
    char *key;
    char *value;
    int is_my_proc = 0;
    FILE *fp;

    sprintf(uid_str, "%u", uid);
    strcpy(filename, dir);
    strcat(filename, "/status");
    fp = fopen(filename, "r");
    if (fp == NULL)
        return 0; // not a wrong, for it's not a PID direntory

    while ((fgets(str, MAXLINE, fp)) != NULL)
    {
        key = strtok(str, ":");
        value = strtok(NULL, ":"); // 继续使用静态分配的，所以NULL
        // 自动除了给定的符号，\n也会停止，符号可以多个",: ;",空格也可
        if (key != NULL && value != NULL)
        {
            while (*value == ' ' || *value == '\t')
                value++;
            // printf("----%s-----%s", key, value);

            if (strcmp(key, "Uid") == 0)
            {
                // printf("-----%s", value);
                if (strstr(value, uid_str) != NULL)
                    is_my_proc = 1;
            }
            if (strcmp(key, "Name") == 0)
                strcpy(proc_name, value);
            if (strcmp(key, "Pid") == 0)
                strcpy(pid, value);
        }
    }
    if (is_my_proc)
    {
        pid[strlen(pid) - 1] = '\0'; // '\0'不算，但是'\n'算strlen里面的
        proc_name[strlen(proc_name) - 1] = '\0';
        printf("%s: %s\n", pid, proc_name);
    }

    fclose(fp);
    return 0;
}
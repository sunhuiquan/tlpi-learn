#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <pwd.h>
#include <stdlib.h>
#include <limits.h>
#include <stdlib.h>

#define MAXNAME 256
#define MAXLINE 4096

typedef struct procnode procnode;
struct procnode
{
    char name[MAXNAME];
    int pid;
    int ppid;
};
int procsz = 0;
int pid_max = 32;
procnode *proc;
/**
 * 原本的想法是直接读 /proc/sys/kernel/pid_max 然后分配这个数，
 * 算了算大约要1GB多点，就很离谱因为实际上才一百多个进程却分配了
 * 多几个数量级，所以用realloc思路，最后只分配了128个数量。
 * 一个GB和几个37KB的差距。。。我一开始脑子有毛病。。。
 * 
 * 后面make_tree的算法就是无脑暴力，因为一共没多少数（逃。
 */

int handle_proc(const char *dir);

int main(int argc, char *argv[])
{
    proc = (procnode *)malloc(sizeof(procnode) * pid_max);

    char dirname[MAXNAME];
    DIR *dirp;
    struct dirent *dir_entry;
    if (argc != 1)
    {
        printf("run it without any arguments.\n");
        exit(0);
    }

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
        handle_proc(dirname);
    }

    for (int i = 0; i < procsz; ++i)
        printf("name:%s|pid:%d|ppid:%d\n", proc[i].name, proc[i].pid, proc[i].ppid);

    return 0;
}

int handle_proc(const char *dir)
{
    char filename[MAXNAME];
    char proc_name[MAXLINE];
    char str[MAXLINE];
    char pid[MAXLINE];
    char ppid[MAXLINE];
    char *key;
    char *value;
    FILE *fp;

    strcpy(filename, dir);
    strcat(filename, "/status");
    fp = fopen(filename, "r");
    if (fp == NULL)
        return 0; /* do nothing and normal return */

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

            if (strcmp(key, "Name") == 0)
                strcpy(proc_name, value);
            if (strcmp(key, "Pid") == 0)
                strcpy(pid, value);
            if (strcmp(key, "PPid") == 0)
                strcpy(ppid, value);
        }
    }

    pid[strlen(pid) - 1] = '\0';
    ppid[strlen(ppid) - 1] = '\0';
    proc_name[strlen(proc_name) - 1] = '\0';
    strcpy(proc[procsz].name, proc_name);
    proc[procsz].pid = atoi(pid);
    proc[procsz].ppid = atoi(ppid);
    ++procsz;

    if (procsz == pid_max)
    {
        proc = realloc(proc, 2 * procsz * sizeof(procnode));
        pid_max = 2 * procsz;
        // printf("---%d\n", procsz);
    }

    fclose(fp);
    return 0;
}
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
#include <unistd.h>

#define MAXNAME 256
#define MAXLINE 4096

int handle_proc(const char *dir, const char *filepath);

int main(int argc, char *argv[])
{
    char dirname[MAXNAME];
    char filepath[MAXNAME];
    DIR *dirp;
    struct dirent *dir_entry;

    if (argc != 2)
    {
        printf("usage: <filepath>\n");
        exit(0);
    }
    strcpy(filepath, argv[1]);
    if (geteuid() != 0)
    {
        printf("This program need root mode\n");
        exit(0);
    }
    printf("Print what processions have opened the path %s:\n", filepath);

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
        handle_proc(dirname, filepath);
    }

    return 0;
}

int handle_proc(const char *dir, const char *filepath)
{
    char dirname[MAXNAME];
    char fdname[MAXNAME];
    char filename[MAXNAME];
    char buf[MAXNAME];
    char str[MAXLINE];
    char proc_name[MAXNAME];
    char pid[MAXLINE];
    struct dirent *dir_entry;
    char *key;
    char *value;
    FILE *fp;

    strcpy(dirname, dir);
    strcat(dirname, "/fd");
    DIR *dirp = opendir(dirname);
    if (dirp == NULL)
        return 0;

    while ((dir_entry = readdir(dirp)) != NULL)
    {
        if (dir_entry->d_type != DT_LNK)
            continue;

        strcpy(fdname, dirname);
        strcat(fdname, "/");
        strcat(fdname, dir_entry->d_name);
        int result = readlink(fdname, buf, MAXNAME - 1);
        // 最后一个是我们要手动加的'\0'
        // printf("%d-%d\n", result, strlen(buf));
        // readlink 返回的后面没加'\0'，不过返回了正确数，所以要手动加'\0'
        if ((result < 0) || (result >= MAXNAME))
            return -1;
        buf[result] = '\0';

        // printf("%s\n", buf);
        if (strcmp(buf, filepath) == 0)
        {
            strcpy(filename, dirname);
            strcat(filename, "/../status");
            fp = fopen(filename, "r");
            if (fp == NULL)
                return 0; /* do nothing and normal return */

            while ((fgets(str, MAXLINE, fp)) != NULL)
            {
                key = strtok(str, ":");
                value = strtok(NULL, ":");
                if (key != NULL && value != NULL)
                {
                    while (*value == ' ' || *value == '\t')
                        value++;
                    // printf("----%s-----%s", key, value);

                    if (strcmp(key, "Name") == 0)
                        strcpy(proc_name, value);
                    if (strcmp(key, "Pid") == 0)
                        strcpy(pid, value);
                }
            }
            fclose(fp);

            pid[strlen(pid) - 1] = '\0';
            proc_name[strlen(proc_name) - 1] = '\0';
            printf("%s (pid:%s)\n", proc_name, pid);
            break;
        }
    }
    closedir(dirp);
    return 0;
}

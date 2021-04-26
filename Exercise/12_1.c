

#include <ctype.h>

/* In-place time of string, s, to remove whitespace */
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

int check_dir(char *dirname, uid_t uid)
{
    char filename[256];
    char linebuf[256];
    char uidstr[128];
    char procname[256];
    char pid[32];
    int keepme;
    char *key;
    char *value;
    FILE *p_file;
    sprintf(uidstr, "%d", uid);
    strcpy(filename, dirname);
    strcat(filename, "/status");
    p_file = fopen(filename, "r");
    if (p_file == NULL)
    {
        return 1; /* just ignore, this is fine I guess */
    }
    keepme = 0;
    while (fgets(linebuf, sizeof(linebuf), p_file) != NULL)
    {
        key = strtok(linebuf, ":");
        if ((strcmp(key, "Uid") == 0))
            printf("a");
        value = strtok(NULL, ":");
        printf("value: %s\n", value);

        if (key != NULL && value != NULL)
        {
            trim(key);
            trim(value);
            if ((strcmp(key, "Uid") == 0) && strstr(value, uidstr) != NULL)
            {
                /* printf("[%s] UID=%s\n", filename, value); */
                keepme = 1;
            }
            if (strcmp(key, "Name") == 0)
            {
                /* printf("[%s] NAME=%s\n", filename, value); */
                strcpy(procname, value);
            }
            if (strcmp(key, "Pid") == 0)
            {
                strcpy(pid, value);
            }
        }
    }
    if (keepme)
    {
        printf("%s: %s\n", pid, procname);
    }
    return 0;
}

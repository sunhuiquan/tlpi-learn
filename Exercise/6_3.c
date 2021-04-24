/*
 *  getenv() putenv() *environ => setenv() unsetenv()（需检查是否多次定义，是的话要移除对该变量的所有定义）
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 4096

extern char **environ;

void pr_env()
{
    for (char **p = environ; *p; p++)
        if (**p)
            printf("%s\n", *p);
    printf("------------------------\n");
}

int my_setenv(const char *name, const char *value, int replace);
int my_unsetenv(const char *name);

int main()
{
    clearenv(); // 清空方便看
    my_setenv("a", "1", 0);
    pr_env();
    my_setenv("b", "1", 0);
    pr_env();
    my_setenv("c", "1", 0);
    pr_env();
    my_setenv("d", "1", 0);
    pr_env();
    my_setenv("a", "2", 0);
    pr_env();
    my_setenv("a", "3", 1);
    pr_env();

    my_unsetenv("a");
    pr_env();
    my_unsetenv("b");
    pr_env();
    my_unsetenv("c");
    pr_env();
    my_unsetenv("d");
    pr_env();
}

int my_setenv(const char *name, const char *value, int replace)
{
    char *env = (char *)malloc(MAXLINE * sizeof(char));
    if (env == NULL)
        return -1;

    printf("%s\n", env);

    env = strcat(env, name);
    env = strcat(env, "=");
    env = strcat(env, value);

    if (environ == NULL)
    {
        environ = (char **)malloc(2 * sizeof(char *));
        environ[0] = env;
        environ[1] = NULL;
        return 0;
    }

    if (getenv(name) == NULL || replace == 1)
        putenv(env);

    return 0;
}

int my_unsetenv(const char *name)
{
    char env_name[MAXLINE];
    memset(env_name, 0, sizeof(env_name));
    for (char **p = environ; *p; p++)
    {
        strcat(env_name, *p);
        int sz = strlen(env_name);
        for (int i = 0; i < sz; ++i)
            if (env_name[i] == '=')
            {
                env_name[i] = '\0';
                break;
            }

        if (strcmp(env_name, name) == 0)
        {
            *p = "";
            return 0;
        }
    }
    return -1;
}
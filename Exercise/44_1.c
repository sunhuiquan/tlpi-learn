#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXBUF 1024

void err_exit(const char *msg)
{
    printf("%s\n", msg);
    exit(EXIT_SUCCESS);
}

int main()
{
    char buf[MAXBUF];
    int parent_w[2], child_w[2];
    ssize_t n;
    char res_buf[MAXBUF];
    size_t sz;

    if (pipe(parent_w) == -1)
        err_exit("pipe");
    if (pipe(child_w) == -1)
        err_exit("pipe");

    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        err_exit("fork");

    case 0:
        /* child */
        if (close(parent_w[1]) == -1)
            err_exit("close");
        if (close(child_w[0]) == -1)
            err_exit("close");

        while (1)
        {
            strcpy(res_buf, "");
            n = read(parent_w[0], buf, MAXBUF);
            if (n == -1)
                err_exit("read");

            if (n == 0)
                break;

            strncpy(res_buf, buf, MAXBUF);
            sz = strlen(res_buf);
            for (int i = 0; i < sz; ++i)
                if (isalpha(res_buf[i]))
                    res_buf[i] = toupper(res_buf[i]);

            if (write(child_w[1], res_buf, strlen(res_buf) + 1) != strlen(buf) + 1)
                err_exit("write");
        }

        if (close(parent_w[0]) == -1)
            err_exit("close");
        if (close(child_w[1]) == -1)
            err_exit("close");

        _exit(EXIT_SUCCESS);
    }

    /* parent */
    if (close(parent_w[0]) == -1)
        err_exit("close");
    if (close(child_w[1]) == -1)
        err_exit("close");

    while (1)
    {
        if (scanf("%s", buf) == -1)
            break;

        if (write(parent_w[1], buf, strlen(buf) + 1) != strlen(buf) + 1)
            err_exit("write");

        strcpy(res_buf, "");
        n = read(child_w[0], buf, MAXBUF);
        if (n == -1)
            err_exit("read");

        printf("%s\n", buf);
    }

    if (close(parent_w[1]) == -1)
        err_exit("close");
    if (close(child_w[0]) == -1)
        err_exit("close");

    waitpid(pid, NULL, 0);

    return 0;
}

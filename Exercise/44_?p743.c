#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 1024

void err_exit(const char *msg)
{
    printf("%s failed\n", msg);
    exit(EXIT_FAILURE);
}

int main()
{
    int pfd[2];
    char buf[MAXLINE];
    if (pipe(pfd) == -1)
        err_exit("pipe");

    FILE *wp = fdopen(pfd[1], "w");
    FILE *rp = fdopen(pfd[0], "r");

    fprintf(wp, "abc");
    fflush(wp);
    // fscanf(rp, "%s", buf);
    read(pfd[0], buf, MAXLINE);

    printf("a\n");

    return 0;
}

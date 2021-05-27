#include <tlpi_hdr.h>
#include <sys/wait.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    int pfd[2];
    size_t read_sz;
    pid_t pid;
    long num, sz;
    double time_sec;
    struct timeval start, end;

    if (argc != 3 || (strcmp(argv[1], "-help") == 0))
    {
        printf("%s usage: block-num blcok-size\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    num = atoi(argv[1]);
    sz = atoi(argv[2]);
    char buf[sz];
    char buf2[sz];
    memset(buf, 1, sizeof(buf)); // junk data

    if (pipe(pfd) == -1)
        errExit("pipe");

    pid = fork();
    switch (pid)
    {
    case -1:
        errExit("fork");

    /* child */
    case 0:
        close(pfd[0]);
        for (int i = 0; i < num; ++i)
            if (write(pfd[1], buf, sz) != sz)
                errExit("write");
        close(pfd[1]);
        _exit(0);
    }

    /* parent */
    close(pfd[1]);

    gettimeofday(&start, 0);
    while (1)
    {
        if ((read_sz = read(pfd[0], buf2, sz)) == -1)
            errExit("read");
        if (read_sz == 0) // ret 0 means the other write ends have all closed
            break;
    }
    gettimeofday(&end, 0);
    close(pfd[0]);
    wait(NULL); // a goog style or just let the init process do it

    time_sec = (double)(1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec) / 1000000;

    printf("%lfseconds and %ldbytes, about %.2lf bytes per second\n",
           time_sec, sz * num, sz * num / time_sec);

    return 0;
}
#include <sys/resource.h>
#include <tlpi_hdr.h>
#include <sys/wait.h>

int main()
{
    struct rusage r;
    long sum = 1;

    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        errExit("fork");

    case 0:
        for (int i = 0; i < 300000000; ++i)
        {
            sum += i / 123;
            sum /= 321;
            sum *= 45;
            if (sum > 10000000)
                sum = 0;
        }
        printf("a caluation for wasting time %ld\n", sum);
        printf("end work\n");
        sleep(10);
        printf("_exit()\n");
        _exit(0);
    }

    sleep(5);
    printf("end work but not terminated\n");
    if (getrusage(RUSAGE_CHILDREN, &r) == -1)
        errExit("getrusage");
    printf("utime %ld.%ld\n", (long)r.ru_utime.tv_sec, (long)r.ru_utime.tv_usec);

    sleep(15);
    printf("child is terminated\n");
    if (getrusage(RUSAGE_CHILDREN, &r) == -1)
        errExit("getrusage");
    printf("utime %ld.%ld\n", (long)r.ru_utime.tv_sec, (long)r.ru_utime.tv_usec);

    printf("after wait for child\n");
    if (wait(NULL) == -1)
        errExit("wait");
    if (getrusage(RUSAGE_CHILDREN, &r) == -1)
        errExit("getrusage");
    printf("utime %ld.%ld\n", (long)r.ru_utime.tv_sec, (long)r.ru_utime.tv_usec);

    return 0;
}
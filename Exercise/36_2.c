#include <tlpi_hdr.h>
#include <sys/resource.h>
#include <sys/wait.h>

void prusage(const struct rusage *ru);

int main(int argc, char *argv[])
{
    struct rusage ru;
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        errExit("fork");

    case 0:
        execvp(argv[1], &argv[1]);
        errExit("execvp");
    }

    if (waitpid(pid, NULL, 0) == -1)
        errExit("waitpid");
    if (getrusage(RUSAGE_CHILDREN, &ru) == -1)
        errExit("getrusage");
    prusage(&ru);

    return 0;
}

void prusage(const struct rusage *ru)
{
    printf("CPU time (secs):         user=%.3f; system=%.3f\n",
           ru->ru_utime.tv_sec + ru->ru_utime.tv_usec / 1000000.0,
           ru->ru_stime.tv_sec + ru->ru_stime.tv_usec / 1000000.0);
    printf("Page reclaims:           %ld\n", ru->ru_minflt);
    printf("Page faults:             %ld\n", ru->ru_majflt);
    printf("Swaps:                   %ld\n", ru->ru_nswap);
    printf("Block I/Os:              input=%ld; output=%ld\n",
           ru->ru_inblock, ru->ru_oublock);
    printf("Signals received:        %ld\n", ru->ru_nsignals);
    printf("IPC messages:            sent=%ld; received=%ld\n",
           ru->ru_msgsnd, ru->ru_msgrcv);
    printf("Context switches:        voluntary=%ld; "
           "involuntary=%ld\n",
           ru->ru_nvcsw, ru->ru_nivcsw);
}

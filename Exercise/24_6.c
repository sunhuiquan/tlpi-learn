#include <tlpi_hdr.h>
#include <wait.h>
#include <assert.h>

int main()
{
    char str[10] = "aaaa";
    pid_t pid = vfork();
    switch (pid)
    {
    case -1:
        errExit("vfork");

    case 0:
        printf("%s\n", str);
        if (close(STDIN_FILENO) == -1)
            errExit("close");
        assert(write(STDIN_FILENO, "c\n", 2) == -1);
        _exit(EXIT_SUCCESS);
        // return 0; It's a bug here, don't use return in a vfork's child process!

    default:
        // waitpid(pid, NULL, 0);
        printf("%s\n", str);
        if (write(STDIN_FILENO, "p\n", 2) != 2)
            errExit("write");
        exit(EXIT_SUCCESS);
    }
}
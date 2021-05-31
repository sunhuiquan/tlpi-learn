#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

static int fd_to_pid[1024];

FILE *mypopen(const char *command, const char *modes);
int mypclose(FILE *stream);

int main()
{
    char buf[1024];
    FILE *fp = popen("echo aaa", "r");
    fscanf(fp, "%s", buf);
    printf("%s\n", buf);

    FILE *fp2 = popen("cat", "w");
    fprintf(fp2, "bbb");
    fprintf(fp2, "ccc");

    /* The fclose() function flushes the stream pointed to by stream
     * (writing any buffered output data using fflush(3)) and closes the
     * underlying file descriptor.
     * Hence you no need to fflush() yoursely, but if you want to write immerately
     * and pclose() or fclose()) is far away, then you must fflush by yourself. */
    pclose(fp);
    pclose(fp2);

    return 0;
}

FILE *mypopen(const char *command, const char *modes)
{
    pid_t pid;
    int pfd[2];
    if (pipe(pfd) == -1)
        return NULL;

    pid = fork();
    switch (pid)
    {
    case -1:
        return NULL;

    case 0:
        /* child */
        if (strcmp(modes, "r") == 0)
        {
            close(pfd[0]);
            if (pfd[1] != STDOUT_FILENO)
            {
                dup2(pfd[1], STDOUT_FILENO);
                close(pfd[1]);
            }
            system(command);
            close(pfd[0]);
            _exit(EXIT_SUCCESS);
        }
        else if (strcmp(modes, "w") == 0)
        {
            close(pfd[1]);
            if (pfd[0] != STDIN_FILENO)
            {
                dup2(pfd[0], STDIN_FILENO);
                close(pfd[0]);
            }
            system(command);
            close(pfd[1]);
            _exit(EXIT_SUCCESS);
        }
        else
            return NULL;
    }

    /* parent */
    if (strcmp(modes, "r") == 0)
    {
        close(pfd[1]);
        fd_to_pid[pfd[0]] = pid;
        return fdopen(pfd[0], "r");
    }
    else if (strcmp(modes, "w") == 0)
    {
        close(pfd[0]);
        fd_to_pid[pfd[1]] = pid;
        return fdopen(pfd[1], "w");
    }

    return NULL;
}

int mypclose(FILE *stream)
{
    int fd = fileno(stream);
    if (fd == -1)
        return -1;

    if (fclose(stream) == -1)
        return -1;

    if (waitpid(fd_to_pid[fd], NULL, 0) == (pid_t)-1)
        return -1;

    return 0;
}
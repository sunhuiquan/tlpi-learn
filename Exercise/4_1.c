#include "tlpi_hdr.h"
#include <fcntl.h>

#define MAXLINE 4096

int main(int argc, char *argv[])
{
    int opt = 0;
    int is_append = 0;
    int tee_fd[MAXLINE];
    int fd_no = 0;
    char temp[MAXLINE];
    ssize_t n = 0;
    /*
    * getopt，getchar都是返回int，因为是EOF是负数，而char类型在不同平台上（甚至
    * 你可以用编译指令指定）并不一定都是有符号的，出于可移植性和约定俗成的规定，注
    * 意这种返回int代替char类型的函数，同时接受返回值的类型也一定要用int。
    */
    while ((opt = getopt(argc, argv, "a-:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            is_append = 1;
            break;
        case '-':
            if (strcmp(optarg, "help") == 0)
            {
                printf("Usage: tee [OPTION]... [FILE]...\n"
                       "Copy standard input to each FILE, and also to standard output.\n"
                       "-a append to the given FILEs, do not overwrite.\n");
            }
            exit(0);
        case '?':
        default:
            exit(0);
        }
    }

    memset(tee_fd, 0, sizeof(tee_fd));
    for (int i = optind; i < argc; ++i)
    {
        if (is_append)
        {
            tee_fd[fd_no++] = open(argv[i], O_CREAT | O_WRONLY | O_APPEND, 0666);
            if (tee_fd[fd_no - 1] < 0)
            {
                exit(1);
            }
        }
        else
        {
            tee_fd[fd_no++] = open(argv[i], O_CREAT | O_WRONLY | O_TRUNC, 0666);
            if (tee_fd[fd_no - 1] < 0)
            {
                exit(1);
            }
        }
    }

    while ((n = read(STDIN_FILENO, temp, MAXLINE)) > 0)
    {
        if (fd_no == 0)
        {
            if (write(STDOUT_FILENO, temp, n) != n)
                exit(1);
        }
        else
        {
            for (int i = 0; i < fd_no; ++i)
                if (write(tee_fd[i], temp, n) != n)
                    exit(1);
        }
    }
    if (n < 0)
        exit(1);

    for (int i = 0; i < fd_no; ++i)
        if (close(i) < 0)
            exit(1);

    return 0;
}
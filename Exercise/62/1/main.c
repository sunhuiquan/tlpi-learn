#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <termios.h>

// Returns  1 if fd is an open file descriptor referring to a terminal;
// otherwise 0 is returned, and errno is set to indicate the error.
int my_isatty(int fd)
{
	struct termios termios;
	if (tcgetattr(fd, &termios) == -1) // 如果没有找到终端或错误就会返回-1
		return 0;
	return 1;
}

int main()
{
	if (my_isatty(STDIN_FILENO))
		printf("STDIN_FILENO is referring to a terminal.\n");
	else
		printf("STDIN_FILENO is not referring to a terminal.\n");
	printf("---------------------------------------------\n");

	if (my_isatty(STDOUT_FILENO))
		printf("STDOUT_FILENO is referring to a terminal.\n");
	else
		printf("STDOUT_FILENO is not referring to a terminal.\n");

	return 0;
}
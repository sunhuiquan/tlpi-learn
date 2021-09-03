#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

#define MAXPASSWORD 256

char *mygetpass(const char *prompt)
{
	static char password[MAXPASSWORD];

	struct termios tp, prev;

	if (write(STDOUT_FILENO, prompt, strlen(prompt)) != strlen(prompt))
		return NULL;

	/* 设置终端属性 */
	if (tcgetattr(STDIN_FILENO, &tp) == -1)
		return NULL;
	prev = tp;

	tp.c_lflag &= ~ECHO;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &tp) == -1)
		return NULL;

	if (read(STDIN_FILENO, password, MAXPASSWORD) < 0)
		return NULL;
	password[strlen(password) - 1] = '\0';
	if (write(STDOUT_FILENO, "\n", 1) != 1)
		return NULL;
	/* 恢复终端属性 */
	if (tcsetattr(STDIN_FILENO, TCSANOW, &prev) == -1)
		return NULL;

	return password;
}
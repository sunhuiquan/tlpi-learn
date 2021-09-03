#include <unistd.h>
#include <termios.h>
#include <tlpi_hdr.h>

int main()
{
	struct termios tm;
#ifdef DEBUG
	struct termios prev;
#endif

	if (isatty(STDIN_FILENO) == 0)
		errExit("STDIN_FILENO is not refering to a terminal.");

	if (tcgetattr(STDIN_FILENO, &tm) == -1)
		errExit("tcgetattr");

#ifdef DEBUG
	prev = tm;
	tm.c_lflag &= ~ICANON;
	tm.c_cc[VMIN] = 1;
	tm.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &tm) == -1)
		errExit("tcsetattr");
#endif

	if (tm.c_lflag & ICANON)
	{
		printf("终端处于规范模式\n");
	}
	else
	{
		printf("终端处于非规范模式\n");
		printf("MIN %ld, TIME %ld\n", (long)tm.c_cc[VMIN], (long)tm.c_cc[VTIME]);
	}

#ifdef DEBUG
	if (tcsetattr(STDIN_FILENO, TCSANOW, &prev) == -1)
		errExit("tcsetattr");
#endif
	return 0;
}
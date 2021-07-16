#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("aaa\n");
	write(STDOUT_FILENO, "bbb\n", 4);

	return 0;
}
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("effictive user id:%ld exec successfully\n", (long)geteuid());
	for (int i = 0; i < argc; ++i)
		printf("%s\n", argv[i]);
}
#include <stdio.h>

void __attribute__((constructor)) mod1_load(void)
{
	printf("mod1: load\n");
}

void __attribute__((destructor)) mod1_unload(void)
{
	printf("mod1: unload\n");
}

void mod1()
{
	printf("mod1\n");
}

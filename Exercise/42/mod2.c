// mod2 依赖 mod1

#include <stdio.h>

void mod1();

void __attribute__((constructor)) mod2_load(void)
{
	printf("mod2: load\n");
}

void __attribute__((destructor)) mod2_unload(void)
{
	printf("mod2: unload\n");
}

void mod2()
{
	printf("mod2\n");
	mod1();
}
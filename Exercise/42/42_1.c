#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <tlpi_hdr.h>

static void invoke(void *handle, const char *fname);

int main()
{
	void *mod1_handle,
		*mod2_handle;

	mod1_handle = dlopen("./libmod1.so.1.0.1", RTLD_NOW | RTLD_GLOBAL);
	// 让mod1可以被后面的mod2依赖使用
	if (mod1_handle == NULL)
		errExit("dlopen");

	mod2_handle = dlopen("./libmod2.so.1.0.1", RTLD_NOW);
	if (mod2_handle == NULL)
		errExit("dlopen");

	invoke(mod2_handle, "mod2");
	invoke(mod1_handle, "mod1");

	printf("mod2 depends on mod1\n");
	dlclose(mod1_handle);

	printf("Main is finished.\n");
	exit(EXIT_SUCCESS);
}

void invoke(void *handle, const char *fname)
{
	const char *err;
	void (*fun)(void);

	(void)dlerror();
	*(void **)(&fun) = dlsym(handle, fname);
	err = dlerror();

	if (err != NULL)
		fatal(err);

	(*fun)();
}
#define _GNU_SOURCE

#include <dlfcn.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
	void *libHandle;
	void (*funcp)(void);
	const char *err;
	Dl_info info;

	if (argc != 3 || strcmp(argv[1], "--help") == 0)
		usageErr("%s lib-path func-name\n", argv[0]);

	printf("%s-%s\n", argv[1], argv[2]);

	libHandle = dlopen(argv[1], RTLD_LAZY);
	if (libHandle == NULL)
		fatal("dlopen: %s", dlerror());

	(void)dlerror();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	funcp = (void (*)(void))dlsym(libHandle, argv[2]);
#pragma GCC diagnostic pop

	err = dlerror();
	if (err != NULL)
		fatal("dlsym: %s", err);

	(void)dlerror();
	dladdr(funcp, &info);
	printf("basename: %s\npathname: %s\nname of nearest run-time symbol with an address <= addr: %llu\nactual address: %s\n", (char *)info.dli_fbase, info.dli_fname, *(unsigned long long *)info.dli_saddr, info.dli_sname);
	err = dlerror();
	if (err != NULL)
		fatal("dlsym: %s", err);

	(*funcp)();

	dlclose(libHandle);

	exit(EXIT_SUCCESS);
}

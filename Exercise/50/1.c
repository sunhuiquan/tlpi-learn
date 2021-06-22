#include <sys/resource.h>
#include <sys/mman.h>
#include <tlpi_hdr.h>

int main()
{
	int memlock;
	void *addr;
	struct rlimit rl;

	if ((memlock = getrlimit(RLIMIT_MEMLOCK, &rl)) == -1)
		errExit("getrlimit");
	printf("memlock limit: %lu bytes\n", (unsigned long)rl.rlim_cur / 1024);

	addr = malloc(rl.rlim_cur / 2 + 1000);
	if (addr == NULL)
		errExit("malloc");
	if (mlock(addr, rl.rlim_cur / 2 + 1000) == -1)
		errExit("mlock");

	addr = malloc(rl.rlim_cur / 2);
	if (addr == NULL)
		errExit("malloc");
	if (mlock(addr, rl.rlim_cur / 2) == -1)
	{
		printf("mlock should wrong here for overflow the limit\n");
		exit(EXIT_SUCCESS);
	}

	return 0;
}
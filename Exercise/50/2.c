#include <sys/mman.h>
#include <tlpi_hdr.h>

int main()
{
	void *addr;
	if ((addr = mmap(NULL, 4096, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0)) == MAP_FAILED)
		errExit("mmap");
	*(int *)addr = 1;
	printf("before %d\n", *(int *)addr);
	if (madvise(addr, 4096, MADV_DONTNEED) == -1)
		errExit("madvise");
	printf("after %d\n", *(int *)addr);
	// MAP_PRIVATE的情况下，丢弃后在内存分页中现在的值重新初始化
	// MAP_SHARED可能会丢弃，这个取决于平台，不具有移植性，可能不会丢弃

	return 0;
}
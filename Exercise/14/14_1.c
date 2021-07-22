#include <tlpi_hdr.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PATHMAX 1024
#define PATHFMT "%s/x%06d"
// 当然chdir改变当前目录进去之后用./相对路径也行，虽然这种写法有点奇葩
#define DIR_MODE (S_IRUSR | S_IWUSR | S_IXUSR | \
				  S_IRGRP | S_IWGRP | S_IXGRP | \
				  S_IROTH | S_IWOTH | S_IXOTH)

// #define TEST

#ifdef TMPFS
#define DIR_NAME "/run/14"
#else
#define DIR_NAME "/tmp/14"
#endif

void shuffle(int array[], int n)
{
	int tmp, j;
	for (int i = 0; i < n; ++i)
	{
		j = rand() % n;
		tmp = array[i];
		array[i] = array[j];
		array[j] = tmp;
	}
}

int main(int argc, char *argv[])
{
	int fd;
	int n = 1000;
	int *unlink_array, *mkdir_array;
	struct timeval start, end;
	double time_sec;
	char filename[PATHMAX];
	struct stat stat_buf;

	if (argc > 2)
	{
		printf("usage: [num 20k-100k]\n"); // 不然时间太短偶然性太大
		exit(EXIT_SUCCESS);
	}
	// 没有考虑错误处理，否则使用atoi的前提是确保给的字符串没问题
	if (argc == 2)
		n = atoi(argv[1]);

	if (stat(DIR_NAME, &stat_buf) != -1)
	{
		if (rmdir(DIR_NAME) == -1)
			errExit("rmdir");
	}
	else if (errno != ENOENT)
		errExit("stat");

	if (mkdir(DIR_NAME, DIR_MODE) == -1)
		errExit("mkdir");

	// 对于大数组开到堆里防止栈溢出导致段错误
	unlink_array = (int *)malloc(sizeof(int) * n);
	if (!unlink_array)
		errExit("malloc");
	for (int i = 0; i < n; ++i)
		unlink_array[i] = i;
	mkdir_array = unlink_array;

#ifdef RANDOM
	mkdir_array = (int *)malloc(sizeof(int) * n);
	if (!mkdir_array)
		errExit("malloc");
	memcpy(mkdir_array, unlink_array, sizeof(int) * n);
	shuffle(mkdir_array, n);
#endif

#ifdef TEST
	for (int i = 0; i < 1000; ++i)
		printf("%d\n", mkdir_array[i]);
	printf("**************************************\n");
	for (int i = 0; i < 1000; ++i)
		printf("%d\n", mkdir_array[i]);
#endif

	gettimeofday(&start, 0);
	for (int i = 0; i < n; ++i)
	{
		sprintf(filename, PATHFMT, DIR_NAME, mkdir_array[i]);
		if ((fd = open(filename, O_CREAT, DIR_MODE)) == -1)
			errExit("open");
		write(fd, "a", 1); // 习题要求填充一个字节
		close(fd);		   // 不然之后的unlink无作用
	}
	for (int i = 0; i < n; ++i)
	{
		sprintf(filename, PATHFMT, DIR_NAME, unlink_array[i]);
		if (unlink(filename) == -1)
			errExit("unlink");
	}
	gettimeofday(&end, 0);
	time_sec = (double)(1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec) / 1000000;

#ifdef TMPFS
	printf("tmpfs - ");
#else
	printf("ext4 - ");
#endif

#ifdef RANDOM
	printf("random - ");
#else
	printf("orderd - ");
#endif

	printf("time: %lf\n", time_sec);

	free(unlink_array);
#ifdef RANDOM
	free(mkdir_array);
#endif

	if (rmdir(DIR_NAME) == -1)
		errExit("rmdir");
	return 0;
}
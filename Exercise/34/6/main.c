#include <stdio.h>
#include <unistd.h>
#include <tlpi_hdr.h>

int main()
{
	pid_t pid;
	char buf[10];

	pid = fork();
	switch (pid)
	{
	case -1:
		errExit("fork");
		break;

	case 0:
		sleep(2); // 确保成为孤儿进程组
		if (read(STDIN_FILENO, buf, 10) < 0)
		{
			if (errno == EIO)
				printf("孤儿进程组read会出现EIO错误\n");
			else
				errExit("read");
		}
		_exit(EXIT_SUCCESS);
		break;

	default:
		break;
	}
	return 0;
}
#include <sys/msg.h>
#include <sys/stat.h>
#include <tlpi_hdr.h>

#define MAX_MTEXT 1024

struct mbuf
{
	long mtype;
	char mtext[MAX_MTEXT];
};

int main(int argc, char *argv[])
{
	int msqid, len;
	struct mbuf msg;

	if (argc != 2)
	{
		printf("%s usage: [msqid]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	msqid = atoi(argv[1]);
	msg.mtype = 1; // system V这个消息队列发送mtype必须大于0
	len = strlen("from message queue");
	strncpy(msg.mtext, "from message queue", MAX_MTEXT);
	for (int i = 0; i < 5; ++i)
	{
		if (msgsnd(msqid, &msg, len, 0) == -1) // 这个函数确保一旦发送成功一定发送全部数据
			errExit("msgsnd");
		sleep(1);
	}

	return 0;
}
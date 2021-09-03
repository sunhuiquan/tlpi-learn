#include <unistd.h>
#include <stdio.h>
#include <tlpi_hdr.h>
#include "mygetpass.h"

int main()
{
	char *p_pass = mygetpass("Password: "); // 注意剥离了最后\n字符，并以NULL结尾
	if (!p_pass)
		errExit("getpass");
	else
		printf("%s\n", p_pass);
	sleep(5);

	return 0;
}
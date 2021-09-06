#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <crypt.h>
#include <pwd.h>
#include <shadow.h>
#include <tlpi_hdr.h>

#define MAX_ARGS_NUMBER 256
#define MAXNAME 256

int main(int argc, char *argv[])
{
	int opt = 0;
	char *user_name = "root";
	char *password, *p, *encrypted;
	struct passwd *pwd;
	struct spwd *spwd;

	while ((opt = getopt(argc, argv, "hu:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			printf("usage: %s [-u user] <program-file> [arg1 arg2 ...]", argv[0]);
			exit(EXIT_FAILURE);
			break;

		case 'u':
			// 请求user密码，和passwd记录对比，切换用户(因为这是个set-user-root程序);
			// 如果没经过-u则默认是root(通过set-user-root)
			// 这里展示了为什么要使用set-user-root，因为只有特权用户才能查看密码记录
			user_name = optarg;
			pwd = getpwnam(user_name);
			if (pwd == NULL)
				fatal("couldn't get password record");
			spwd = getspnam(user_name);
			if (spwd == NULL && errno == EACCES)
				fatal("no permission to read shadow password file");
			if (spwd != NULL)					/* If there is a shadow password record */
				pwd->pw_passwd = spwd->sp_pwdp; /* Use the shadow password */

			password = getpass("Password: ");
			encrypted = crypt(password, pwd->pw_passwd);
			// 这是一种单向加密，之后比较的是加密后的值，第二个是密钥用来提高安全性
			for (p = password; *p != '\0';) // 覆盖在内存上的值
				*p++ = '\0';
			if (encrypted == NULL)
				errExit("crypt");

			if (strcmp(encrypted, pwd->pw_passwd))
			{
				printf("Password wrong.\n");
				exit(EXIT_SUCCESS);
			}

			printf("Login successfully.\n");
			// 包括会把saved-id设置成新的有效id，从而永久抛弃恢复特权的能力
			if (setreuid(getuid(), getuid()) == -1)
				errExit("setreuid");
			break;

		case '?':
		default:
			printf("usage: %s [-u user] <program-file> [arg1 arg2 ...]", argv[0]);
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (optind > argc - 1)
	{
		printf("usage: %s [-u user] <program-file> [arg1 arg2 ...]", argv[0]);
		exit(EXIT_FAILURE);
	}

	char *pargs[MAX_ARGS_NUMBER];
	int i = 0;
	for (; optind + i < argc; ++i)
		pargs[i] = argv[optind + i];
	pargs[i] = NULL;

	printf("user name: %s\n", user_name);
	if (execv(argv[optind], pargs) == -1)
		errExit("execl");

	return 0;
}

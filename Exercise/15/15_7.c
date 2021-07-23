/** 	An easy implementation of chattr(1)
 * NAME
 *		chattr - change file attributes on a Linux file system
 * 
 * SYNOPSIS
 *		chattr [-f] [-p] [mode] files...
 * 
 * DESCRIPTION
 *		-f		Suppress most error messages.
 *		-p		Set the file's project number.
 *		The format of a symbolic mode is +-=[aAcCdDeFijPsStTu]
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>

void usage_error()
{
	printf("usage: chattr [mode] files...\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int fd, modify, attr, new_attr = 0;

	modify = argv[1][0];
	for (int i = 1; argv[1][i] != '\0'; ++i)
	{
		switch (argv[1][i])
		{
		case 'a':
			new_attr |= FS_APPEND_FL;
			break;

		case 'A':
			new_attr |= FS_NOATIME_FL;
			break;

		case 'c':
			new_attr |= FS_COMPR_FL;
			break;

		case 'd':
			new_attr |= FS_NODUMP_FL;
			break;

		case 'D':
			new_attr |= FS_DIRSYNC_FL;
			break;

		case 'i':
			new_attr |= FS_IMMUTABLE_FL;
			break;

		case 'j':
			new_attr |= FS_JOURNAL_DATA_FL;
			break;

		case 's':
			new_attr |= FS_SECRM_FL;
			break;

		case 'S':
			new_attr |= FS_SYNC_FL;
			break;

		case 't':
			new_attr |= FS_NOTAIL_FL;
			break;

		case 'T':
			new_attr |= FS_TOPDIR_FL;
			break;

		case 'u':
			new_attr |= FS_UNRM_FL;
			break;

		default:
			usage_error();
			break;
		}
	}

	for (int i = 2; i < argc; ++i)
	{
		if ((fd = open(argv[i], O_RDONLY)) == -1)
		{
			fprintf(stderr, "%s open failed\n", argv[i]);
			continue;
		}

		if (ioctl(fd, FS_IOC_GETFLAGS, &attr) == -1)
		{
			fprintf(stderr, "ioctl get flags failed\n");
			close(fd);
			continue;
		}

		switch (modify)
		{
		case '+':
			attr |= new_attr;
			break;

		case '-':
			attr &= (~new_attr);
			break;

		case '=':
			attr = new_attr;
			break;

		default:
			usage_error();
			break;
		}

		if (ioctl(fd, FS_IOC_SETFLAGS, &attr) == -1)
			fprintf(stderr, "ioctl set flags failed\n");

		close(fd);
	}

	return 0;
}
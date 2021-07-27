/*	An easy implementation of nftw()
	using opendir(), readdir(), closedir(), stat()
*/

#define _XOPEN_SOURCE 600
#include <tlpi_hdr.h>
#include <ftw.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

typedef int (*func_t)(const char *filename,
					  const struct stat *status, int flag,
					  struct FTW *info);

int _ftw_rec(const char *pathname, func_t func, int descriptors, int flag, int level)
{
	int ret_value = 0;
	DIR *dir;
	struct dirent *dirent;
	struct stat sbuf;
	char path[PATH_MAX] = "";
	char temp[PATH_MAX];
	char cwd[PATH_MAX];
	int dev;
	struct FTW info;
	int f;

	if (stat(pathname, &sbuf) == -1)
		return -1;
	dev = sbuf.st_dev;

	if (flag & FTW_CHDIR)
	{
		if ((getcwd(cwd, PATH_MAX)) == NULL)
			return -1;
		if (chdir(pathname) == -1)
			return -1;
	}
	else
	{
		if (pathname[strlen(pathname) - 1] != '/')
			sprintf(path, "%s/", pathname); // 这里是为了确保有个'/'，因为'//'都等同于一个'/'没关系
		else
			sprintf(path, "%s", pathname);
	}

	info.base = strlen(path);
	info.level = level;

	if ((dir = opendir(pathname)) == NULL)
		return -1;
	--descriptors;
	if (descriptors < 0)
		return -1;
	while (1)
	{
		errno = 0;
		if ((dirent = readdir(dir)) == NULL)
		{
			if (errno)
				continue;
			break;
		}

		strcpy(temp, path);
		if (!strcmp(temp, "./") && !strcmp(dirent->d_name, "."))
			strcpy(temp, ".");
		else
			strncat(temp, dirent->d_name, PATH_MAX);
		if (flag & FTW_PHYS)
		{
			if (lstat(temp, &sbuf) == -1)
				continue;
		}
		else if (stat(temp, &sbuf) == -1)
			continue;

		if (sbuf.st_dev != dev)
			continue;

		if (strcmp(dirent->d_name, ".."))
		{
			if (S_ISDIR(sbuf.st_mode))
			{
				f = FTW_D;
				if (strlen(temp) > 2 && temp[strlen(temp) - 1] == '.' && temp[strlen(temp) - 2] == '/')
				{
					if (!(level == 0 && temp[0] == '/' && *dirent->d_name == '.'))
						if ((temp[strlen(temp) - 3] != '.' || temp[strlen(temp) - 4] != '.'))
							continue;
				}

				errno = 0;
				if (access(temp, R_OK) == -1)
				{
					if (errno)
						continue;
					f = FTW_DNR;
				}

				if (!(flag & FTW_DEPTH))
					if (func(temp, &sbuf, f, NULL) != 0)
					{
						if (flag & FTW_CHDIR)
							if (chdir(cwd) == -1)
								return -1;
						return ret_value;
					}

				if (strcmp(dirent->d_name, "."))
				{
					if ((ret_value = _ftw_rec(temp, func, descriptors, flag, level + 1)) != 0)
					{
						if (flag & FTW_CHDIR)
							if (chdir(cwd) == -1)
								return -1;
						return ret_value;
					}
				}

				if (flag & FTW_DEPTH)
				{
					if (f != FTW_DNR)
						f = FTW_DP;
					if (func(temp, &sbuf, f, NULL) != 0)
					{
						if (flag & FTW_CHDIR)
							if (chdir(cwd) == -1)
								return -1;
						return ret_value;
					}
				}
			}
			else
			{
				f = FTW_F;

				if (flag & FTW_PHYS && S_ISLNK(sbuf.st_mode))
					f = FTW_SL;

				if (!(flag & FTW_PHYS) && S_ISLNK(sbuf.st_mode))
					f = FTW_SLN;

				if (func(temp, &sbuf, f, &info) != 0)
				{
					if (flag & FTW_CHDIR)
						if (chdir(cwd) == -1)
							return -1;
					return ret_value;
				}
			}
		}
	}
	if (flag & FTW_CHDIR)
		if (chdir(cwd) == -1)
			return -1;
	closedir(dir);
	return 0;
}

int mynftw(const char *dir, func_t func, int descriptors, int flag)
{
	return _ftw_rec(dir, func, descriptors, flag, 0);
}

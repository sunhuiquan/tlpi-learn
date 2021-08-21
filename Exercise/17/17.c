#include <acl/libacl.h>
#include <sys/acl.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <tlpi_hdr.h>
#include <sys/stat.h>

int get_perms(char *perms, acl_entry_t *entry, acl_permset_t *permset)
{
	if (acl_get_permset(*entry, permset) == -1)
		return -1;

	int permVal = acl_get_perm(*permset, ACL_READ);
	if (permVal == -1)
		return -1;
	strcat(perms, (permVal == 1) ? "r" : "-");
	permVal = acl_get_perm(*permset, ACL_WRITE);
	if (permVal == -1)
		return -1;
	strcat(perms, (permVal == 1) ? "w" : "-");
	permVal = acl_get_perm(*permset, ACL_EXECUTE);
	if (permVal == -1)
		return -1;
	strcat(perms, (permVal == 1) ? "x" : "-");

	return 0;
}

char *use_mask(char *str, char *mask)
{
	int len = (strlen(str) < strlen(mask)) ? strlen(str) : strlen(mask);
	for (int i = 0; i < len; ++i)
		if (str[i] != mask[i]) // char是值类型，比的没问题
			str[i] = '-';
	return str;
}

int main(int argc, char *argv[])
{
	int is_user = 0, is_find = 0, is_holder = 0, need_mask = 0, entryId;
	char name[256], user[256], group[256];
	struct passwd *pwd;
	struct group *grp;
	acl_t acl;
	acl_entry_t entry;
	acl_tag_t tag;
	uid_t *uidp;
	gid_t *gidp;
	acl_permset_t permset;
	char perms[10] = "";
	char obj_perms[10] = "";
	char other_perms[10] = "";
	char mask[10] = "";
	struct stat sbuf;

	if (argc != 4 || !strcmp(argv[1], "-help"))
	{
		printf("usage: %s <u|g> <user|group> <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (stat(argv[3], &sbuf) == -1)
		errExit("stat");

	pwd = getpwuid((int)sbuf.st_uid);
	if (pwd == NULL)
		errExit("getpwuid fail or can't find");
	else
		strncpy(user, pwd->pw_name, 256);

	grp = getgrgid((int)sbuf.st_gid);
	if (grp == NULL)
		errExit("getpwuid fail or can't find");
	else
		strncpy(group, pwd->pw_name, 256);

	if (!strcmp(argv[1], "u"))
	{
		is_user = 1;
		if (strspn(argv[2], "0123456789") == strlen(argv[2])) // 全数字
		{
			errno = 0;
			pwd = getpwuid(atoi(argv[2]));
			if (pwd == NULL)
			{
				if (errno == 0) // 找不到说明之前那个就是字符型的名
					strncpy(name, argv[2], 256);
				else
					errExit("getpwuid");
			}
			else
				strncpy(name, pwd->pw_name, 256);
		}
		else
			strncpy(name, argv[2], 256);
	}
	else if (!strcmp(argv[1], "g"))
	{
		if (strspn(argv[2], "0123456789") == strlen(argv[2])) // 全数字
		{
			errno = 0;
			grp = getgrgid(atoi(argv[2]));
			if (grp == NULL)
			{
				if (errno == 0) // 找不到说明之前那个就是字符串形式的名
					strncpy(name, argv[2], 256);
				else
					errExit("getpwuid");
			}
			else
				strncpy(name, grp->gr_name, 256);
		}
		else
			strncpy(name, argv[2], 256);
	}
	else
	{
		printf("usage: %s <u|g> <user|group> <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	acl = acl_get_file(argv[3], ACL_TYPE_ACCESS);
	if (acl == NULL)
		errExit("acl_get_file");

	for (entryId = ACL_FIRST_ENTRY;; entryId = ACL_NEXT_ENTRY)
	{

		if (acl_get_entry(acl, entryId, &entry) != 1)
			break;

		if (acl_get_tag_type(entry, &tag) == -1)
			errExit("acl_get_tag_type");

		if (is_user && tag == ACL_USER)
		{
			uidp = acl_get_qualifier(entry);
			if (uidp == NULL)
				errExit("acl_get_qualifier");

			pwd = getpwuid(*uidp);
			if (pwd == NULL)
				errExit("getpwuid errno or can't find that user");

			if (acl_free(uidp) == -1)
				errExit("acl_free");

			if (!strcmp(pwd->pw_name, name))
			{
				if (get_perms(perms, &entry, &permset) == -1)
					errExit("get_perms");
				need_mask = 1;
				is_find = 1;
				break;
			}
		}
		else if (!is_user && tag == ACL_GROUP)
		{
			gidp = acl_get_qualifier(entry);
			if (gidp == NULL)
				errExit("acl_get_qualifier");

			grp = getgrgid(*gidp);
			if (grp == NULL)
				errExit("getpwuid errno or can't find that user");

			if (acl_free(gidp) == -1)
				errExit("acl_free");

			if (!strcmp(grp->gr_name, name))
			{
				if (get_perms(perms, &entry, &permset) == -1)
					errExit("get_perms");
				need_mask = 1;
				is_find = 1;
				break;
			}
		}

		if (tag == ACL_USER_OBJ && is_user && !strcmp(name, user))
		{
			if (get_perms(obj_perms, &entry, &permset) == -1)
				errExit("get_perms");
			is_holder = 1;
		}
		if (tag == ACL_GROUP_OBJ && !is_user && !strcmp(name, group))
		{
			if (get_perms(obj_perms, &entry, &permset) == -1)
				errExit("get_perms");
			need_mask = 1;
			is_holder = 1;
		}
		if (tag == ACL_OTHER)
			if (get_perms(other_perms, &entry, &permset) == -1)
				errExit("get_perms");
		if (tag == ACL_MASK)
			if (get_perms(mask, &entry, &permset) == -1)
				errExit("get_perms");
	}

	if (acl_free(acl) == -1)
		errExit("acl_free");

	if (is_find)
	{
		if (need_mask)
			use_mask(perms, mask);
		printf("%s\n", perms);
	}
	else if (is_holder)
	{
		if (need_mask)
			use_mask(obj_perms, mask);
		printf("%s\n", obj_perms);
	}
	else
		printf("%s\n", other_perms);

	return 0;
}
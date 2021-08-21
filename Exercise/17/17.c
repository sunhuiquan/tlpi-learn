#include <acl/libacl.h>
#include <sys/acl.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <tlpi_hdr.h>

int get_perms(char *perms, acl_entry_t *entry, acl_permset_t *permset, int permVal)
{
	if (acl_get_permset(*entry, permset) == -1)
		return -1;

	permVal = acl_get_perm(*permset, ACL_READ);
	if (permVal == -1)
		return -1;
	fprintf(perms, "%s %c", perms, (permVal == 1) ? 'r' : '-');
	permVal = acl_get_perm(*permset, ACL_WRITE);
	if (permVal == -1)
		return -1;
	fprintf(perms, "%s %c", perms, (permVal == 1) ? 'w' : '-');
	permVal = acl_get_perm(*permset, ACL_EXECUTE);
	if (permVal == -1)
		return -1;
	fprintf(perms, "%s %c\n", perms, (permVal == 1) ? 'x' : '-');
}

int main(int argc, char *argv[])
{
	int is_user = 0;
	char *name;
	struct passwd *pwd;
	struct group *grp;
	acl_t acl;
	acl_type_t type;
	acl_entry_t entry;
	acl_tag_t tag;
	uid_t *uidp;
	gid_t *gidp;
	acl_permset_t permset;
	char *acl_name;
	int entryId, permVal, opt;
	char perms[10] = "";
	char obj_perms[10] = "";
	int is_find = 0;

	if (argc != 3 || !strcmp(argv[1], "-help"))
	{
		printf("usage: %s <u|g> <user|group> <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

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
					name = argv[2];
				else
					errExit("getpwuid");
			}
			else
				name = pwd->pw_name;
		}
		else
			name = argv[2];
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
					name = argv[2];
				else
					errExit("getpwuid");
			}
			else
				name = grp->gr_name;
		}
		else
			name = argv[2];
	}
	else
	{
		printf("usage: %s <u|g> <user|group> <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("%s\n", name);

	// ------------------------------------------------------------------

	acl = acl_get_file(argv[3], type);
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

			pwd = getpwuid(uidp);
			if (pwd == NULL)
				errExit("getpwuid errno or can't find that user");

			if (acl_free(uidp) == -1)
				errExit("acl_free");

			if (!strcmp(pwd->pw_name, name))
			{
				if (get_perms(perms, &entry, &permset, permVal) == -1)
					errExit("get_perms");
				is_find = 1;
				break;
			}
		}
		else if (!is_user && tag == ACL_GROUP)
		{
			gidp = acl_get_qualifier(entry);
			if (gidp == NULL)
				errExit("acl_get_qualifier");

			grp = getgrgid(atoi(argv[2]));
			if (grp == NULL)
				errExit("getpwuid errno or can't find that user");

			if (acl_free(gidp) == -1)
				errExit("acl_free");

			if (!strcmp(grp->gr_name, name))
			{
				if (get_perms(perms, &entry, &permset, permVal) == -1)
					errExit("get_perms");
				is_find = 1;
				break;
			}
		}

		if (tag == ACL_USER_OBJ || tag == ACL_GROUP_OBJ)
			if (get_perms(obj_perms, &entry, &permset, permVal) == -1)
				errExit("get_perms");
	}

	if (acl_free(acl) == -1)
		errExit("acl_free");

	if (is_find)
		printf("%s\n", perms);
	else
		printf("%s\n", obj_perms);

	return 0;
}
#include <acl/libacl.h>
#include <sys/acl.h>
#include "tlpi_hdr.h"

char *userNameFromId(uid_t uid);
uid_t userIdFromName(const char *name);
char *groupNameFromId(gid_t gid);
gid_t groupIdFromName(const char *name);

static void
usageError(char *progName)
{
	fprintf(stderr, "Usage: %s [-d] filename\n", progName);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	acl_t acl;
	acl_type_t type;
	acl_entry_t entry;
	acl_tag_t tag;
	uid_t *uidp;
	gid_t *gidp;
	acl_permset_t permset;
	char *name;
	int entryId, permVal, opt;

	type = ACL_TYPE_ACCESS;
	while ((opt = getopt(argc, argv, "d")) != -1)
	{
		switch (opt)
		{
		case 'd':
			type = ACL_TYPE_DEFAULT;
			break;
		case '?':
			usageError(argv[0]);
		}
	}

	if (optind + 1 != argc)
		usageError(argv[0]);

	acl = acl_get_file(argv[optind], type);
	if (acl == NULL)
		errExit("acl_get_file");

	/* Walk through each entry in this ACL */

	for (entryId = ACL_FIRST_ENTRY;; entryId = ACL_NEXT_ENTRY)
	{

		if (acl_get_entry(acl, entryId, &entry) != 1)
			break; /* Exit on error or no more entries */

		/* Retrieve and display tag type */

		if (acl_get_tag_type(entry, &tag) == -1)
			errExit("acl_get_tag_type");

		printf("%-12s", (tag == ACL_USER_OBJ) ? "user_obj" : (tag == ACL_USER)	  ? "user"
														 : (tag == ACL_GROUP_OBJ) ? "group_obj"
														 : (tag == ACL_GROUP)	  ? "group"
														 : (tag == ACL_MASK)	  ? "mask"
														 : (tag == ACL_OTHER)	  ? "other"
																				  : "???");

		/* Retrieve and display optional tag qualifier */

		if (tag == ACL_USER)
		{
			uidp = acl_get_qualifier(entry);
			if (uidp == NULL)
				errExit("acl_get_qualifier");

			name = userNameFromId(*uidp);
			if (name == NULL)
				printf("%-8d ", *uidp);
			else
				printf("%-8s ", name);

			if (acl_free(uidp) == -1)
				errExit("acl_free");
		}
		else if (tag == ACL_GROUP)
		{
			gidp = acl_get_qualifier(entry);
			if (gidp == NULL)
				errExit("acl_get_qualifier");

			name = groupNameFromId(*gidp);
			if (name == NULL)
				printf("%-8d ", *gidp);
			else
				printf("%-8s ", name);

			if (acl_free(gidp) == -1)
				errExit("acl_free");
		}
		else
		{
			printf("         ");
		}

		/* Retrieve and display permissions */

		if (acl_get_permset(entry, &permset) == -1)
			errExit("acl_get_permset");

		permVal = acl_get_perm(permset, ACL_READ);
		if (permVal == -1)
			errExit("acl_get_perm - ACL_READ");
		printf("%c", (permVal == 1) ? 'r' : '-');
		permVal = acl_get_perm(permset, ACL_WRITE);
		if (permVal == -1)
			errExit("acl_get_perm - ACL_WRITE");
		printf("%c", (permVal == 1) ? 'w' : '-');
		permVal = acl_get_perm(permset, ACL_EXECUTE);
		if (permVal == -1)
			errExit("acl_get_perm - ACL_EXECUTE");
		printf("%c", (permVal == 1) ? 'x' : '-');

		printf("\n");
	}

	if (acl_free(acl) == -1)
		errExit("acl_free");

	exit(EXIT_SUCCESS);
}

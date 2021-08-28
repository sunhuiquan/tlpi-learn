#include <unistd.h>
#include <utmpx.h>
#include <string.h>
#include <utmpx.h>
#include <paths.h>
#include <tlpi_hdr.h>

#define MAX_USERLINE 32 // ut_line's size
#define MAX_USERNAME 32 // ut_user's size

// Return the login name of the user.
char *my_getlogin()
{
	static char name[MAX_USERNAME];
	char *linename;
	struct utmpx *ut;

	if ((linename = ttyname(STDIN_FILENO)) == NULL)
		return NULL;

	setutxent();
	while ((ut = getutxent()) != NULL)
		if (!strncmp(linename + 5, ut->ut_line, MAX_USERLINE))
		{
			strncpy(name, ut->ut_user, MAX_USERNAME);
			endutxent();
			return name;
		}
	endutxent();
	return NULL;
}

int main()
{
	// Test:
	char *name;
	if ((name = my_getlogin()) == NULL)
		errExit("getlogin");

	printf("%s\n", name);

	return 0;
}
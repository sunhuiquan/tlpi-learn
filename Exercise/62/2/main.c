#include <stdio.h>
#include <tlpi_hdr.h>
#include "myttyname.h"

/* The function ttyname() returns a pointer to a pathname on success.
 * On error, NULL is returned, and errno is set appropriately.
 */

int main()
{
	char *str = my_ttyname(STDIN_FILENO);
	if (str == NULL)
		errExit("ttyname");
	printf("%s\n", str);
	printf("------------------\n");
	str = ttyname(STDIN_FILENO);
	if (str == NULL)
		errExit("ttyname");
	printf("%s\n", str);

	return 0;
}
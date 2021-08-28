// LOGIN(3)                                          Linux Programmer's Manual                                          LOGIN(3)

// NAME
//        login, logout - write utmp and wtmp entries

// SYNOPSIS
//        #include <utmp.h>

//        void login(const struct utmp *ut);

//        int logout(const char *ut_line);

//        Link with -lutil.

// DESCRIPTION
//        The utmp file records who is currently using the system.  The wtmp file records all logins and logouts.  See utmp(5).

//        The function login() takes the supplied struct utmp, ut, and writes it to both the utmp and the wtmp file.

//        The function logout() clears the entry in the utmp file again.

//    GNU details
//        More  precisely, login() takes the argument ut struct, fills the field ut->ut_type (if there is such a field) with the
//        value USER_PROCESS, and fills the field ut->ut_pid (if there is such a field) with  the  process  ID  of  the  calling
//        process.   Then  it tries to fill the field ut->ut_line.  It takes the first of stdin, stdout, stderr that is a termi‐
//        nal, and stores the corresponding pathname minus a possible leading /dev/ into this field, and then writes the  struct
//        to  the  utmp file.  On the other hand, if no terminal name was found, this field is filled with "???"  and the struct
//        is not written to the utmp file.  After this, the struct is written to the wtmp file.

//        The logout() function searches the utmp file for an entry matching the ut_line argument.  If a record is found, it  is
//        updated  by  zeroing  out  the  ut_name and ut_host fields, updating the ut_tv timestamp field and setting ut_type (if
//        there is such a field) to DEAD_PROCESS.

// RETURN VALUE
//        The logout() function returns 1 if the entry was successfully written to the database, or 0 if an error occurred.

// FILES
//        /var/run/utmp
//               user accounting database, configured through _PATH_UTMP in <paths.h>

//        /var/log/wtmp
//               user accounting log file, configured through _PATH_WTMP in <paths.h>

// ATTRIBUTES
//        For an explanation of the terms used in this section, see attributes(7).

//        ┌──────────┬───────────────┬──────────────────────┐
//        │Interface │ Attribute     │ Value                │
//        ├──────────┼───────────────┼──────────────────────┤
//        │login(),  │ Thread safety │ MT-Unsafe race:utent │
//        │logout()  │               │ sig:ALRM timer       │
//        └──────────┴───────────────┴──────────────────────┘
//        In the above table, utent in race:utent signifies that if any of the  functions  setutent(3),  getutent(3),  or  endu‐
//        tent(3)  are  used  in  parallel in different threads of a program, then data races could occur.  login() and logout()
//        calls those functions, so we use race:utent to remind users.

// CONFORMING TO
//        Not in POSIX.1.  Present on the BSDs.

// NOTES
//        Note that the member ut_user of struct utmp is called ut_name in BSD.  Therefore, ut_name is defined as an  alias  for
//        ut_user in <utmp.h>.

// SEE ALSO
//        getutent(3), utmp(5)

// COLOPHON
//        This page is part of release 5.05 of the Linux man-pages project.  A description of the project, information about re‐
//        porting bugs, and the latest version of this page, can be found at https://www.kernel.org/doc/man-pages/.

// GNU                                                       2017-09-15                                                 LOGIN(3)

// #define _GNU_SOURCE /* updwtmpx */

// #include <sys/types.h>
// #include <unistd.h>
// #include <utmpx.h>
// #include <utmp.h>
// #include <paths.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// static void helpAndLeave(const char *progname, int status);
// static void pexit(const char *fname);

// static void _login(const struct utmpx *ut);

// char *utmp_file = _PATH_UTMP,
// 	 *wtmp_file = _PATH_WTMP;

// int main(int argc, char *argv[])
// {
// 	if (argc == 1)
// 		helpAndLeave(argv[0], EXIT_FAILURE);

// 	char *username = argv[1];

// 	if (argc > 2)
// 		utmp_file = argv[2];

// 	if (argc > 3)
// 		wtmp_file = argv[3];

// 	utmpname(utmp_file);
// 	struct utmpx ut;
// 	strncpy(ut.ut_user, username, __UT_NAMESIZE);

// 	_login(&ut);

// 	printf("Username %s has been logged in.\n", username);
// 	exit(EXIT_SUCCESS);
// }

// static void
// _login(const struct utmpx *ut)
// {
// 	struct utmpx record;
// 	strncpy(record.ut_user, ut->ut_user, __UT_NAMESIZE);

// 	/* fill in basic information */
// 	record.ut_type = USER_PROCESS;
// 	record.ut_pid = getpid();

// 	int stds[] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO},
// 		i;
// 	char *tty;

// 	/* iterate over stdin, stdout and stderr (in that order), trying to find the
// 	 * name of the controlling terminal of the calling process. */
// 	for (i = 0; i < 2; ++i)
// 	{
// 		if ((tty = ttyname(stds[1])) != NULL)
// 			break;
// 	}

// 	if (tty != NULL)
// 	{
// 		/* strip leading slash if any */
// 		if (tty[0] == '/')
// 			++tty;

// 		char *p;
// 		/* remove `dev/` prefix */
// 		if ((p = strchr(tty, '/')) != NULL)
// 			tty = p + 1;

// 		strncpy(record.ut_line, tty, __UT_LINESIZE);
// 	}

// 	/* commit the record to the utmp file */
// 	if (pututxline(&record) == NULL)
// 		pexit("pututxline");

// 	/* commit the record to the wtmp file */
// 	updwtmpx(wtmp_file, &record);
// }

// static void
// helpAndLeave(const char *progname, int status)
// {
// 	FILE *stream = stderr;

// 	if (status == EXIT_SUCCESS)
// 		stream = stdout;

// 	fprintf(stream, "Usage: %s <username> [utmp_file] [wtmp_file]\n", progname);
// 	exit(status);
// }

// static void
// pexit(const char *fname)
// {
// 	perror(fname);
// 	exit(EXIT_FAILURE);
// }

// And logout, logwtmp functions to implement.

# TLPI_learn_note
My notes and solutions for The Linux Programing Interface(《LINUX/LINUX系统编程手册》笔记和解答)


官方README
README for code in The Linux Programming Interface

Gidday!

This is the code for the book "The Linux Programming Interface"
and this is a note from me, the author, Michael Kerrisk.

For instructions on building the programs, see the file BUILDING.

For notes on changes that have been made to the code since it was
published in the book, see the file CHANGES.


Source code licensing
=====================

All complete programs provided in this distribution are covered by
the GNU General Public License (Version 3), a copy of which is
contained in the file COPYING.gpl-v3, which should have arrived with
this tarball.  The library functions (in the lib/ directory) are
covered by the GNU Lesser General Public License (Version 3); see the
files COPYING.lgpl-v3 and COPYING.gpl-v3 provided with this tarball.


A note on the source code
=========================

The source code is available in two versions: "dist" and "book".
The "book" version contains the program source files as published in
the book. The source files in the "dist" version contain extra code
beyond that published in the book. The differences between the "dist"
and "book" versions are as follows:

a) The "dist" versions of some programs contain extra comments.
   These additional comments were stripped out of the printed version
   to make the published versions of the programs shorter. (The book
   itself contains text describing the operation of the programs.)

b) In a few cases, some changes have been incorporated into the
   "dist" versions to make it possible to compile programs on UNIX
   implementations other than Linux, so that you can try them out
   on other implementations if you wish.  Where necessary, the
   additional code is conditionally compiled using the following
   platform-specific macros:

        __linux__       Defined on Linux
        __sun           Defined on Solaris
        __FreeBSD__     Defined on FreeBSD
        __NetBSD__      Defined on NetBSD
        __OpenBSD__     Defined on OpenBSD
        __APPLE__       Defined on Mac OS X
        __hpux          Defined on HP-UX
        __osf__         Defined on Tru64 UNIX (formerly DEC OSF1)
        __sgi           Defined on Irix
        _AIX            Defined on AIX

c) In the "dist" version, some programs have extra functionality beyond
   that in the "book" versions. Where this is significant, comments in
   the programs explain the differences.


Subdirectories
==============

Under the 'tlpi' directory are a number of subdirectories. Each
subdirectory corresponds to one or more chapters of the book.
The following paragraphs give brief notes on the contents of
each subdirectory.

Note that in some cases, files are (hard) linked to appear in more than
one directory. This is particularly the case for each of the files in
the 'lib' directory, most of which are also linked in the directory
of the chapter relating to that file.

Directory       Files for Chapter...

lib             This contains library routines used by other
                programs. The tlpi_hdr.h and error_functions.*
                files are located here.

progconc        3 (System Programming Concepts)

fileio          4 and 5 (File I/O)

proc            6 (Processes)

memalloc        7 (Memory Allocation)

users_groups    8 (Users and Groups)

proccred        9 (Process Credentials)

time            10 (Time)

syslim          11 (System Limits and Options)

sysinfo         12 (System and Process Information)

filebuff        13 (File I/O Buffering)

filesys         14 (File Systems)

files           15 (File Attributes)

xattr           16 (Extended Attributes)

acl             17 (Access Control Lists)

dirs_links      18 (Directories and Links)

inotify         19 (Monitoring File Events)

signals         20 to 22 (Signals)

timers          23 (Timers and Sleeping)

procexec        24 (Process Creation), 25 (Process Termination),
                26 (Monitoring Child Processes), 27 (Program Execution),
                and 28 (Further Details on Process Creation and Program
                Execution)

threads         29 to 33 (POSIX Threads)

pgsjc           34 (Process Groups, Sessions, and Job Control)

procpri         35 (Process Priorities and Scheduling)

procres         36 (Process Resources)

daemons         37 (Daemons)

cap             39 (Capabilities)

loginacct       40 (Login Accounting)

shlibs          41 and 42 (Shared Libraries)

pipes           44 (Pipes and FIFOs)

svipc           45 (System V IPC)

svmsg           46 (System V Message Queues)

svsem           47 (System V Semaphores)

svshm           48 (System V Shared Memory)

mmap            49 (Memory Mappings)

vmem            50 (Virtual Memory Operations)

pmsg            52 (POSIX Message Queues)

psem            53 (POSIX Semaphores)

pshm            54 (POSIX Shared Memory)

filelock        55 (File Locking)

sockets         56 to 61 (Sockets and Network Programming)

tty             62 (Terminals)

altio           63 (Alternative I/O Models)

pty             64 (Pseudoterminals)

getopt          Appendix B: Parsing Command-Line Options
官方BUILDING帮助
BUILDING notes for code in The Linux Programming Interface

PLEASE TAKE NOTE!
=================

If you have difficulty building the programs, then the problem is most
likely one of the following:

a) A configuration issue on your system (e.g., do you have the 'libacl'
   library installed?).
b) You are using a system with an older Linux kernel or an older
   version of glibc. If this is so, your system may not provide a
   more recent system call or library function, or the glibc headers
   may not provide a needed function declaration or constant
   definition.

I can't really help with fixing these problems. However, do take
a look at the online code FAQ (http://man7.org/tlpi/code/faq.html)
for some frequently asked questions about the code. You may find
the answer to your problem there.

If, after ensuring that the problem is not a configuration issue on your
system (did you try compiling the program(s) on a different system?) and
checking the FAQ, you still have a problem *and* you have a solution, then
please do let me know, and I will publish the fix on the web site.


Unpacking the code tarball
==========================

Unpacking the tarball with the command

        tar xfz tlpi-YYMMDD-xxxx.tar.gz

will create a directory tree 'tlpi-xxxx' containing all of the source code
("xxxx" will be either "dist" or "book" depending on which version of the
source code tarball you downloaded).


Building the programs on Linux
==============================

(For instructions on building the programs on other UNIX
implementations, see the notes lower down in this file.)

The methods described below involve the use of 'make'. If you need
more information on 'make', try the command 'man make' or 'info make'.


Method A - Building all programs in all directories
---------------------------------------------------

Go into the 'tlpi' subdirectory, and type 'make':

        cd tlpi-xxxx
        make

This will build all programs in all subdirectories.


Method B - Build programs in individual directories
---------------------------------------------------

1) First, build the library used by all programs:

        cd lib
        make            # This will make libtlpi.a and place
                        # it in the parent directory

2) Build programs using the 'Makefile' in each subdirectory.

In each subdirectory, there is a file called 'Makefile' that can be
used with the 'make' command to build the programs in that directory.
To build a particular program you can simply say:

        make progname

where 'progname' is one of the C (main) programs in the directory.
Thus, to build the executable for the program 'copy.c' you would use
the command:

        make copy

Each makefile also contains the following special targets (which are
named according to the usual 'make' conventions):

all     This target will build all programs in the subdirectory.
        Simply enter the command:

                make all

clean   This target cleans up, by removing all executables and object
        files created by the makefile:

                make clean


Building the programs on other UNIX implementations
===================================================

I have gone to some effort to ensure that, where possible, the programs
work on other UNIX implementations also.

Obviously, some of the example programs employ Linux-specific features,
and won't compile on other UNIX implementations. As a first indication
of what works on Linux, and what may work elsewhere, look in the
makefiles in each directory. Most of the makefiles define two macros:

LINUX_EXE       These are programs that probably won't work
                on anything other than Linux.

GEN_EXE         These programs may compile on other UNIX implementations.
                I say "may", because of course not all implementations
                provide exactly the same features. The presence of a
                program in this list indicates that it compiles and runs
                on at least some UNIX implementations other than Linux.


Instructions
------------

1. Edit Makefile.inc in the 'tlpi' root directory to modify the
   definitions of the CFLAGS and LDLIBS macros (and possibly other
   macros depending on your version of make(1)) as appropriate.
   Probably you'll need to define CFLAGS as follows:

        CFLAGS += -g -I${TLPI_INCL_DIR}

   The setting of LDLIBS is a bit harder to determine. As well as
   listing the libary for this book, you should include '-l' linker
   options for any other libraries that the programs may need.
   For example, the following is suitable on Solaris 8:

        LDLIBS = ${TLPI_LIB} -lsocket -lnsl -lrt -ldl -lm -lresolv

   **** NOTE:   Under the 'tlpi' root directory you'll find a few sample
                replacement files for 'Makefile.inc', named
                'Makefile.inc.*'. You may be able to simply copy the
                appropriate file to 'Makefile.inc'.

2. Try the following to build all of the GEN_EXE programs:

        cd tlpi-xxxx
        make -k allgen

   The '-k' option tells 'make' to build as much as possible, so that if
   a particular program won't compile, 'make' goes on to attempt to
   build the remaining programs.
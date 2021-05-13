/* Use thread-specific data to implement dirname and basename's thread-safe version */

#ifndef TS_DIRNAME_H
#define TS_DIRNAME_H

// thread-safe by using thread specific data
char *ts_dirname(char *pathname);

// thread-safe by using thread local stroge
char *tls_dirname(char *pathname);

#endif
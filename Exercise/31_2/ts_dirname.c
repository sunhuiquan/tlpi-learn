#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "ts_dirname.h"

#define MAXLINE 1024

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t key;

void destr(void *arg)
{
    free(arg);
}

void create_key_for_ts_dirname(void)
{
    pthread_key_create(&key, destr);
}

char *ts_dirname(char *pathname)
{
    pthread_once(&once, create_key_for_ts_dirname);

    char *buf = pthread_getspecific(key);
    if (buf == NULL)
    {
        buf = (char *)malloc(sizeof(char) * MAXLINE);
        if (buf == NULL)
            return NULL;
        pthread_setspecific(key, buf);
    }
    strncpy(buf, pathname, MAXLINE);

    if (buf != NULL && strlen(buf) != 0)
    {
        int i = strlen(buf) - 1;
        if (buf[i] == '/')
            buf[i] = '\0';

        for (; i >= 0; --i)
        {
            if (buf[i] == '/')
            {
                buf[i] = '\0';
                if (i == 0)
                {
                    strcpy(buf, ".");
                    return buf;
                }
                return buf;
            }
        }
    }

    strcpy(buf, ".");
    return buf;
}

// tls -- thread local stroge
static __thread char buf[MAXLINE];

char *tls_dirname(char *pathname)
{
    strncpy(buf, pathname, MAXLINE);

    if (buf != NULL && strlen(buf) != 0)
    {
        int i = strlen(buf) - 1;
        if (buf[i] == '/')
            buf[i] = '\0';

        for (; i >= 0; --i)
        {
            if (buf[i] == '/')
            {
                buf[i] = '\0';
                if (i == 0)
                {
                    strcpy(buf, ".");
                    return buf;
                }
                return buf;
            }
        }
    }
    strcpy(buf, ".");
    return buf;
}
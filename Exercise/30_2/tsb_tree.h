/* A bin serach tree with thread safe requirement
 * and tsbTree means thread safe binary tree.
 */

#ifndef TSB_TREE_H
#define TSB_TREE_H

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAXLINE 1024

typedef enum Boolean Boolean;
enum Boolean
{
    True,
    False
};

typedef struct tsbTree *ptsbTree;
struct tsbTree
{
    char *key;
    void *value;
    ptsbTree left_ptsb;
    ptsbTree right_ptsb;
    pthread_mutex_t mutex;
};

int tsb_initialize(ptsbTree tree);
int tsb_add(ptsbTree tree, char *key, void *value);
int tsb_delete(ptsbTree tree, char *key);
Boolean tsb_lookup(char *key, void **value);

#endif
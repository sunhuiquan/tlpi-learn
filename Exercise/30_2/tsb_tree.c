#include "tsb_tree.h"

#define RET_CHECK(r)   \
    do                 \
    {                  \
        if (r != 0)    \
        {              \
            errno = r; \
            return -1; \
        }              \
    } while (0);

/* It's  a totally thread safe function, for we only can initialize one time for a tree */
int tsb_initialize(ptsbTree tree)
{
    // Use errno to fint wrong and set the errno as r thougn it will reduce effective.
    int r = pthread_mutex_init(&tree->mutex, NULL);
    RET_CHECK(r);

    tree = (ptsbTree)malloc(sizeof(struct tsbTree));
    if (tree == 0)
        return -1;

    tree->key = NULL;
    tree->value = NULL;
    tree->left_ptsb = tree->right_ptsb = NULL;

    return 0;
}

int tsb_add(ptsbTree tree, char *key, void *value)
{
    if (tree == NULL || key == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    int r = pthread_mutex_lock(&tree->mutex);
    RET_CHECK(r);

    if (tree->key == NULL)
    {
        tree->key = key;
        tree->value = value;
        r = pthread_mutex_unlock(&tree->mutex);
        RET_CHECK(r);
        return 0;
    }

    r = pthread_mutex_unlock(&tree->mutex);
    RET_CHECK(r);

    ptsbTree pnode = (ptsbTree)malloc(sizeof(struct tsbTree));
    if (pnode == NULL)
        return -1;
    pnode->key = key;
    pnode->value = value;
    pnode->left_ptsb = pnode->right_ptsb = NULL;
    r = pthread_mutex_init(&tree->mutex, NULL);
    RET_CHECK(r);

    ptsbTree pre;
    while (tree)
    {
        r = pthread_mutex_lock(&tree->mutex);
        RET_CHECK(r);

        int compare = strcmp(tree->key, key);
        if (compare > 0)
        {
            pre = tree;
            tree = tree->left_ptsb;
            if (tree == NULL)
                pre->left_ptsb = pnode;
        }
        else if (compare < 0)
        {
            pre = tree;
            tree = tree->right_ptsb;
            if (tree == NULL)
                pre->right_ptsb = pnode;
        }
        else
        {
            r = pthread_mutex_unlock(&tree->mutex);
            RET_CHECK(r);
            break;
        }

        r = pthread_mutex_unlock(&tree->mutex);
        RET_CHECK(r);
    }

    return 0;
}

Boolean tsb_lookup(char *key, void **value)
{
    return 0;
}
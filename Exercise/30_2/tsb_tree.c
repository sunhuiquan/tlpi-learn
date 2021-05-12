#include "tsb_tree.h"

/* It's  a totally thread safe function, for there's no shared varible */
int tsb_initialize(ptsbTree tree)
{
    int r; // We use errno to tell wrong hence set the
    // errno as r thougn it will reduce effective.
    tree = (ptsbTree)malloc(sizeof(struct tsbTree));
    if (tree == 0)
        return -1;

    tree->key = NULL;
    tree->value = NULL;
    tree->left_ptsb = NULL;
    tree->right_ptsb = NULL;

    r = pthread_mutex_init(&tree->mutex, NULL);
    if (r != 0)
    {
        errno = r;
        return -1;
    }

    return 0;
}

int tsb_add(ptsbTree tree, char *key, void *value)
{
    int r;
    if (key == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    r = pthread_mutex_lock(&tree->mutex);
    if (r != 0)
    {
        errno = r;
        return -1;
    }
    if (tree->key == NULL)
    {
        tree->key = key;
        tree->value = value;
        return 0;
    }
    r = pthread_mutex_unlock(&tree->mutex);
    if (r != 0)
    {
        errno = r;
        return -1;
    }

    ptsbTree pnode = (ptsbTree)malloc(sizeof(struct tsbTree));
    pnode->key = key;
    pnode->value = value;
    pnode->left_ptsb = NULL;
    pnode->right_ptsb = NULL;
    r = pthread_mutex_init(&pnode->mutex, NULL);
    if (r != 0)
    {
        errno = r;
        return -1;
    }

    ptsbTree p = tree;
    while (1)
    {
        r = pthread_mutex_lock(&p->mutex);
        if (r != 0)
        {
            errno = r;
            return -1;
        }

        if (strncmp(p->key, pnode->key, MAXLINE) == 1) // To avoid C-str terminates without null character
        {
            if (p->left_ptsb)
                p = p->left_ptsb;
            else
            {
                p->left_ptsb = pnode;
                break;
            }
        }
        else
        {
            if (p->right_ptsb)
                p = p->right_ptsb;
            else
            {
                p->right_ptsb = pnode;
                break;
            }
        }

        r = pthread_mutex_unlock(&p->mutex);
        if (r != 0)
        {
            errno = r;
            return -1;
        }
    }

    return 0;
}

int tsb_delete(ptsbTree tree, char *key)
{
    if (key == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    ptsbTree p = tree, del_node = NULL, cur_node = NULL;
    int compare;
    while (p)
    {
        compare = strncmp(p->key, key, MAXLINE);
        if (compare == 0)
        {
            del_node = p;
            // to do : if(){}
        }
        else if (compare == 1)
        {
            p = p->left_ptsb;
        }
        else
        {
            p = p->right_ptsb;
        }
    }

    if (del_node == NULL)
    {
        // to do errno
        return -1;
    }

    return 0;
}

Boolean tsb_lookup(char *key, void **value)
{
    return 0;
}
#include "kv.h"

#include <stdlib.h>
#include <string.h>

// 利用空头结点简化分类(这样不用考虑作为头结点的情况)
kv_dsa *init_kv()
{
	kv_dsa *kv = (kv_dsa *)malloc(sizeof(struct kv_dsa));
	if (kv)
	{
		kv->key[0] = '\0';
		// 防止后面值比较恰好相同(概率非常非常小)
		kv->next = NULL;
	}
	return kv;
}

int add_kv(kv_dsa *kv, char *key, char *value)
{
	if (kv == NULL)
		return -1;

	kv_dsa *p = kv;
	for (; p->next; p = p->next)
	{
		if (!strcmp(p->next->key, key))
			return -1;
	}

	p->next = (kv_dsa *)malloc(sizeof(struct kv_dsa));
	if (p->next == NULL)
		return -1;
	p->next->next = NULL;
	strncpy(p->next->key, key, MAXBUF);
	strncpy(p->next->value, value, MAXBUF);
	return 0;
}

int delete_kv(kv_dsa *kv, char *key)
{
	kv_dsa *p = kv, *pre = NULL;
	for (; p; p = p->next)
	{
		// 空头结点所以pre一定不是NULL
		if (!strcmp(p->key, key))
		{
			pre->next = p->next;
			free(p);
			return 0;
		}
		pre = p;
	}
	return -1;
}

int change_kv(kv_dsa *kv, char *key, char *value)
{
	kv_dsa *p = kv;
	for (; p; p = p->next)
	{
		if (!strcmp(p->key, key))
		{
			strncpy(p->value, value, MAXBUF);
			return 0;
		}
	}
	return -1;
}

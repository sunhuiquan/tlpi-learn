#ifndef KV_H
#define KV_H

/* 
 * key-value we use string-string as follows
 * apple : pingguo
 * banana : xiangjiao
 * canada : jianada
 */

#define MAXBUF 32
typedef struct kv_dsa kv_dsa;
struct kv_dsa
{
	char key[MAXBUF];
	char value[MAXBUF];
	kv_dsa *next;
};

kv_dsa *init_kv();

int add_kv(kv_dsa *kv, char *key, char *value);

int delete_kv(kv_dsa *kv, char *key);

int change_kv(kv_dsa *kv, char *key, char *value);

#endif

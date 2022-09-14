#include <u.h>
#include <libc.h>
#include "asif.h"

/* k&r 2e pp.143-145 */

static int
hash(char *s)
{
	char c;
	uint h;

	for(h=0, c=*s++; c!=0; c=*s++)
		h = c + 31 * h;
	return h % Hashsz;
}

static HPair *
find(HTab *ht, char *key, HPair **prev)
{
	HPair *k, *kp;

	if(ht == nil)
		return nil;
	kp = &ht->b[hash(key)];
	if(prev != nil)
		*prev = kp;
	for(k=kp->next; k!=nil; kp=k, k=k->next){
		if(prev != nil)
			*prev = kp;
		if(strcmp(key, k->key) == 0)
			return k;
	}
	return nil;
}

static char *
bytestr(u32int key)
{
	static char u[5];

	u[0] = key;
	u[1] = key >> 8;
	u[2] = key >> 16;
	u[3] = key >> 24;
	u[4] = 0;
	return u;
}

int
htremove(HTab *ht, char *key)
{
	HPair *k, *kp;

	if((k = find(ht, key, &kp)) == nil){
		werrstr("no such key \"%s\"", key);
		return -1;
	}
	kp->next = k->next;
	free(k->key);
	free(k);
	return 0;
}

int
htremovel(HTab *ht, u32int key)
{
	return htremove(ht, bytestr(key));
}

void *
htget(HTab *ht, char *key)
{
	HPair *k;

	if((k = find(ht, key, nil)) == nil){
		werrstr("no such key \"%s\"", key);
		return nil;
	}
	return k->val;
}

void *
htgetl(HTab *ht, u32int key)
{
	return htget(ht, bytestr(key));
}

int
htput(HTab *ht, char *key, void *val)
{
	HPair *kp, *k;

	if((k = find(ht, key, &kp)) != nil){
		k->val = val;
		return 0;
	}
	k = emalloc(sizeof *k);
	k->key = estrdup(key);
	k->val = val;
	kp->next = k;
	return 0;
}

int
htputl(HTab *ht, u32int key, void *val)
{
	return htput(ht, bytestr(key), val);
}

HTab *
htalloc(void)
{
	HTab *ht;

	ht = emalloc(sizeof *ht);
	return ht;
}

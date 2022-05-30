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
	return h;
}

void *
htget(HTab *ht, char *key)
{
	HPair *k;

	if(ht == nil)
		return nil;
	for(k=ht->b[hash(key)].next; k!=nil; k=k->next)
		if(strcmp(key, k->key) == 0)
			return k->val;
	return nil;
}

void
htput(HTab *ht, char *key, void *val)
{
	HPair *kp, *k;

	if(ht == nil)
		return;
	for(kp=&ht->b[hash(key)], k=kp->next; k!=nil; kp=k, k=k->next)
		if(strcmp(key, k->key) == 0){
			k->val = val;
			return;
		}
	k = emalloc(sizeof *k);
	k->key = estrdup(key);
	k->val = val;
	kp->next = k;
}

HTab *
htalloc(void)
{
	HTab *ht;

	ht = emalloc(sizeof *ht);
	return ht;
}

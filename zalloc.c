#include <u.h>
#include <libc.h>
#include "asif.h"

/* no nodes are ever freed, left to be reclaimed by the kernel on exit */
enum{
	Ninc = 128,
};

static void
zlink(Znode *p, Zpool *z)
{
	assert(p != nil && z != nil);
	p->prev = z->prev;
	p->next = &z->Znode;
	z->prev->next = p;
	z->prev = p;
}

static Znode *
zunlink(Zpool *z)
{
	Znode *q;

	assert(z != nil && z->next != nil);
	q = z->next;
	q->next->prev = &z->Znode;
	z->next = q->next;
	q->prev = q->next = nil;
	return q;
}

static void
zfeed(Zpool *z)
{
	int n;
	uchar *p, *q;

	assert(z != nil && z->elsize > 0);
	if(z->next != z)
		return;
	n = z->elsize + sizeof *z;
	p = emalloc(Ninc * n);		// see comment
	for(q=p; q<p+Ninc*n; q+=n)
		zlink((Znode*)q, z);
}

void
zfree(Znode *p, Zpool *z)
{
	if(p == nil)
		return;
	assert(z != nil);
	memset(p->data, 0, z->elsize);
	zlink(p, z);
}

void *
zalloc(Zpool *z)
{
	Znode *p;

	zfeed(z);
	p = zunlink(z);
	return p->data;
}

Zpool *
znew(int elsize)
{
	Zpool *z;

	assert(elsize > 0);
	z = emalloc(sizeof *z);
	z->elsize = elsize;
	z->next = z->prev = z;
	zfeed(z);
	return z;
}

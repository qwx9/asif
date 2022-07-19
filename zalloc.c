#include <u.h>
#include <libc.h>
#include "asif.h"

/* "simple" "slab allocation" free lists: no memory is ever freed, left to be
 * reclaimed by the kernel on exit; should be able to detect some bugs, but
 * still convoluted. keeps track of used nodes to be able to reap them all at
 * once.
 */

typedef struct Zhouse Zhouse;
typedef struct Znode Znode;
typedef struct Zhdr Zhdr;
typedef struct Ztail Ztail;
struct Zhdr{
	Zhouse *z;
	Znode *next;
	Znode *prev;
};
struct Znode{
	Zhdr;
	void data[];
};
struct Ztail{
	Zhouse *z;
};
struct Zhouse{
	Zpool;
	Znode used;
	Znode;
};

enum{
	Ninc = 2048,
};

static Ztail *
n2t(Znode *p)
{
	return (Ztail *)((uchar *)p + sizeof(Zhdr) + p->z->elsize);
}

static Znode *
v2n(uchar *data)
{
	return (Znode *)(data - sizeof(Zhdr));
}

static void
zcheckpool(Znode *p, Zhouse *z)
{
	Ztail *t;

	assert(p == nil || p != z);
	assert(z != nil && z->elsize > 0);
	if(p != nil){
		assert(p->z == z);
		t = n2t(p);
		assert(t->z == z);
	}
}

static void
zlink(Znode *p, Znode *l, int clr)
{
	if(clr){
		p->next->prev = p->prev;
		p->prev->next = p->next;
	}
	p->prev = l->prev;
	p->next = l;
	l->prev->next = p;
	l->prev = p;
}

void
znuke(Zpool *zp)
{
	Zhouse *z;

	if(zp == nil)
		return;
	z = (Zhouse *)zp;
	/* prev → z → NEXT → … → PREV → next; contents must be zeroed */
	z->used.prev->next = z->next;
	z->next->prev = z->used.prev;
	z->next = z->used.next;
	z->used.next->prev = &z->Znode;
	z->used.next = z->used.prev = &z->used;
}

static Znode *
zpop(Zhouse *z)
{
	Znode *p;

	p = z->next;
	assert(p != z);
	zlink(p, &z->used, 1);
	return p;
}

static void
_zfree(Znode *p)
{
	Zhouse *z;

	if(p == nil)
		return;
	z = p->z;
	zcheckpool(p, z);
	memset(p->data, 0, z->elsize);
	zlink(p, &z->Znode, 1);
}

void
zfree(void *data)
{
	if(data == nil)
		return;
	_zfree(v2n(data));
}

static void
zfeed(Zhouse *z)
{
	ulong n;
	uchar *u, *v;
	Znode *p;
	Ztail *t;

	if(z->next != z)
		return;
	zcheckpool(nil, z);
	n = sizeof(Zhdr) + z->elsize + sizeof *t;
	u = emalloc(Ninc * n);
	for(v=u; v<u+Ninc*n; v+=n){
		p = (Znode *)v;
		p->z = z;
		t = n2t(p);
		t->z = z;
		zlink(p, &z->Znode, 0);
	}
}

void *
zalloc(Zpool *zp)
{
	Znode *p;
	Zhouse *z;

	z = (Zhouse *)zp;
	zfeed(z);
	p = zpop(z);
	memset(p->data, 0, z->elsize);
	return p->data;
}

Zpool *
znew(usize elsize)
{
	Zhouse *z;

	assert(elsize > 0);
	z = emalloc(sizeof *z);
	z->elsize = elsize;
	z->next = z->prev = z;
	z->used.next = z->used.prev = &z->used;
	zfeed(z);
	return z;
}

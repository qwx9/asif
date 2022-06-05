#include <u.h>
#include <libc.h>
#include "asif.h"

/* no nodes are ever freed, left to be reclaimed by the kernel on exit
 * got more complicated, but should be able to detect some bugs...
 * unsuitable for very large collections probably */

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
	Znode;
};

enum{
	Ninc = 128,
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
zlink(Znode *p, Zhouse *z)
{
	zcheckpool(p, z);
	p->prev = z->prev;
	p->next = &z->Znode;
	z->prev->next = p;
	z->prev = p;
}

static Znode *
zpop(Zhouse *z)
{
	Znode *p;

	p = z->next;
	assert(p != z);
	zcheckpool(p, z);
	p->next->prev = &z->Znode;
	z->next = p->next;
	p->prev = p->next = nil;
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
	zlink(p, z);
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
		zlink(p, z);
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
	zfeed(z);
	return z;
}

#include <u.h>
#include <libc.h>
#include "asif.h"

static Vecarr *
checksize(Vecarr *v)
{
	assert(v->len * v->elsz <= v->bufsz);
	if((v->len + 1) * v->elsz < v->bufsz)
		return v;
	v->buf = erealloc(v->buf, v->bufsz * 2, v->bufsz);
	v->bufsz *= 2;
	v->tail = (uchar *)v->buf + v->len * v->elsz;
	return v;
}

void
vecarrnuke(Vecarr *v)
{
	if(v == nil)
		return;
	memset(v->buf, 0xf1, v->bufsz);		/* FIXME: just let pool do it? */
	free(v->buf);
	v->buf = nil;
	v->tail = nil;
}

usize
vecarrindexof(Vecarr *v, void *p)
{
	uintptr x;

	assert(v != nil && p >= v->buf && p < (uchar *)v->buf + v->elsz * v->len);
	x = (uchar *)p - (uchar *)v->buf;
	if(x % v->elsz != 0)
		panic("vecarrindexof: invalid pointer");
	return x / v->elsz;
}

void *
vecarrget(Vecarr *v, usize i)
{
	void *n;

	if(i >= v->len || v->len == 0){
		werrstr("vecarrget %#p[%zd]: out of bounds", v, i);
		return nil;
	}
	n = (uchar*)v->buf + i * v->elsz;
	return n;
}

void *
vecarrpoptail(Vecarr *v)
{
	void *n;

	assert(v->len > 0);
	n = v->tail;
	v->len--;
	v->tail = (uchar *)v->tail - v->elsz;
	return n;
}

void *
vecarrp(Vecarr *v, usize i)
{
	void *p;

	p = (uchar *)v->buf + v->elsz * i;
	assert(p >= v->buf && p < (uchar *)v->buf + v->elsz * v->len);
	return p;
}

void
vecarrresize(Vecarr *v, usize nel)
{
	assert(v->elsz > 0);
	if(nel == v->len)
		return;
	v->buf = erealloc(v->buf, nel * v->elsz, v->bufsz);
	v->len = nel;
	v->bufsz = v->len * v->elsz;
	v->tail = (uchar *)v->buf + v->bufsz;
}

void *
vecarrcopy(Vecarr *v, void *p, usize *ip)
{
	usize i;
	uchar *t;

	assert((uintptr)p > v->elsz);
	checksize(v);
	i = v->elsz * v->len;
	t = (uchar *)v->buf + i;
	memcpy(t, p, v->elsz);
	v->len++;
	if(ip != nil)
		*ip = i / v->elsz;
	v->tail = t + v->elsz;
	return (void *)t;
}

Vecarr
vecarr(usize elsz, usize len)
{
	Vecarr v;

	assert(elsz > 0);
	memset(&v, 0, sizeof v);
	v.elsz = elsz;
	v.len = 0;
	v.bufsz = (len > 2 ? len : 2) * elsz;
	v.buf = emalloc(v.bufsz);
	v.tail = v.buf;
	return v;
}

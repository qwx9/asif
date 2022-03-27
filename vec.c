#include <u.h>
#include <libc.h>
#include "asif.h"

static void
shardlink(Vector *v, VShard *s)
{
	s->next = &v->vl;
	s->prev = v->vl.prev;
	s->prev->next = s;
	v->vl.prev = s;
//	fprint(2, "%#p len %d link: ", v, v->len);
//	for(s=v->vl.next; s!=&v->vl; s=s->next)
//		fprint(2, "%#p[%d,%zd] → ", s, s->len, (s->head - s->p) / v->elsz);
//	fprint(2, "%#p\n", &v->vl);
}

static void
shardunlink(Vector *v, VShard *s)
{
	memset(s->p, 0, Shardsz * v->elsz);
	s->head = s->p;
	s->prev->next = s->next;
	s->next->prev = s->prev;
//	fprint(2, "%#p len %d link: ", v, v->len);
//	for(s=v->vl.next; s!=&v->vl; s=s->next)
//		fprint(2, "%#p[%d,%zd] → ", s, s->len, (s->head - s->p) / v->elsz);
//	fprint(2, "%#p\n", &v->vl);
}

static VShard *
shard(Vector *v)
{
	VShard *s;

	s = emalloc(sizeof *s);
	s->p = emalloc(Shardsz * v->elsz);
	s->head = s->p;
//	fprint(2, "%#p len %d shard %#p\n", v, v->len, s);
	shardlink(v, s);
	return s;
}

void
vecfree(Vector *v)
{
	VShard *s, *t;

//	fprint(2, "%#p len %d free\n", v, v->len);
	free(v->tmp);
	for(s=v->vl.next; s!=&v->vl; s=t){
		t = s->next;
		free(s->p);
		free(s);
	}
	free(v);
}

static void *
shardpop(Vector *v, VShard *s, int i)
{
	uchar *p;
	VShard *t;

//	fprint(2, "%#p len %d pop %#p[%d,%zd] i %d\n", v, v->len, s, s->len, (s->head - s->p) / v->elsz, i);
	assert(s != &v->vl);
	assert(i >= 0 && i < s->len);
	p = s->head + i * v->elsz;
	assert(p < s->p + Shardsz * v->elsz);
	memcpy(v->tmp, p, v->elsz);
	s->len--;
	v->len--;
	if(i == 0)
		s->head += v->elsz;
	for(t=v->vl.next; t->len == 0 && t!=&v->vl; t=t->next){
		shardunlink(v, t);
		shardlink(v, t);
	} 
	assert(s->head >= s->p && s->head < s->p + Shardsz * v->elsz);
	return v->tmp;
}

void *
vechpop(Vector *v)
{
	VShard *s;

	if(v->len <= 0)
		return nil;
	s = v->vl.next;
//	fprint(2, "vechpop %#p len %d s %#p len %d Δhead %zd prev %#p next %#p\n",
//		v, v->len, s, s->len, s->head - s->p, s->prev, s->next);
	/* FIXME: crash here with first el.len == 0; second .len == 64
	 * moving source */
	assert(s != &v->vl && s->len > 0 && s->head - s->p < Shardsz * v->elsz);
	return shardpop(v, s, 0);
}

void *
vectpop(Vector *v)
{
	VShard *s;

	if(v->len <= 0)
		return nil;
	for(s=v->vl.prev; s != &v->vl && s->len == 0; s=s->prev)
		;
//	fprint(2, "vectpop %#p len %d s %#p len %d Δhead %zd prev %#p next %#p\n",
//		v, v->len, s, s->len, s->head - s->p, s->prev, s->next);
	assert(s != &v->vl);
	return shardpop(v, s, s->len - 1);
}

void
vecpush(Vector *v, void *e)
{
	uchar *p;
	VShard *s;

	for(s=v->vl.next; s != &v->vl; s=s->next)
		if(s->len + (s->head - s->p) / v->elsz < Shardsz)
			break;
//	fprint(2, "%#p len %d push s %#p[%d,%zd]\n", v, v->len, s, s->len, (s->head - s->p) / v->elsz);
	if(s == &v->vl)
		s = shard(v);
	p = s->head + s->len * v->elsz;
	assert(p >= s->p && p < s->p + Shardsz * v->elsz);
	memcpy(p, e, v->elsz);
	s->len++;
	v->len++;
}

void *
vecget(Vector *v, int i)
{
	VShard *s;

	assert(i >= 0 && i < v->len);
	for(s=v->vl.next; s != &v->vl && i > s->len; s=s->next)
		i -= s->len;
	assert(s != &v->vl);
	assert(s->len > 0);
	assert(i >= 0 && i < s->len);
	return s->head + i * v->elsz;
}

Vector *
vec(int elsz)
{
	Vector *v;

	assert(elsz > 0);
	v = emalloc(sizeof *v);
	v->elsz = elsz;
	v->vl.next = v->vl.prev = &v->vl;
	v->tmp = emalloc(elsz);
	shard(v);
	return v;
}

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
}

static void
shardunlink(Vector *v, VShard *s)
{
	memset(s->p, 0, Shardsz * v->elsz);
	s->head = s->p;
	s->prev->next = s->next;
	s->next->prev = s->prev;
}

static VShard *
shard(Vector *v)
{
	VShard *s;

	s = emalloc(sizeof *s);
	s->p = emalloc(v->elsz * Shardsz);
	s->head = s->p;
	shardlink(v, s);
	return s;
}

void
vecfree(Vector *v)
{
	VShard *s, *t;

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

	assert(s != &v->vl);
	assert(i >= 0 && i < s->len);
	p = (uchar *)s->head + i * v->elsz;
	memcpy(v->tmp, p, v->elsz);
	s->len--;
	v->len--;
	if(i == 0)
		s->head = (uchar *)s->head + v->elsz;
	if(s->len == 0){
		shardunlink(v, s);
		shardlink(v, s);
	}
	return v->tmp;
}

void *
vechpop(Vector *v)
{
	uchar *p;
	VShard *s;

	if(v->len <= 0)
		return nil;
	s = v->vl.next;
	assert(s != &v->vl);
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
	assert(s != &v->vl);
	return shardpop(v, s, s->len - 1);
}

void
vecpush(Vector *v, void *e)
{
	uchar *p;
	VShard *s;

	for(s=v->vl.prev; s != &v->vl && s->len >= Shardsz; s=s->prev)
		;
	if(s == &v->vl)
		s = shard(v);
	p = (uchar *)s->head + s->len * v->elsz;
	memcpy(p, e, v->elsz);
	s->len++;
	v->len++;
}

void *
vecget(Vector *v, int i)
{
	uchar *p;
	VShard *s;

	assert(i >= 0 && i < v->len);
	for(s=v->vl.next; s != &v->vl && i > s->len; s=s->next)
		i -= s->len;
	assert(s != &v->vl);
	assert(s->len > 0);
	assert(i >= 0 && i < s->len);
	return (uchar *)s->head + i * v->elsz;
}

Vector *
vec(int elsz)
{
	Vector *v;
	VShard *s;

	assert(elsz > 0);
	v = emalloc(sizeof *v);
	v->elsz = elsz;
	v->vl.next = v->vl.prev = &v->vl;
	v->tmp = emalloc(elsz);
	shard(v);
	return v;
}

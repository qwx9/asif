#include <u.h>
#include <libc.h>
#include "asif.h"

enum{
	VAdefsize = 1024,
};

void
vfree(VArray *v)
{
	if(v == nil)
		return;
	free(v->p);
	free(v);
}

void
vinsert(VArray *v, char *u)
{
	int off;

	assert(v != nil && u != nil);
	off = v->n * v->elsize;
	if(v->n++ >= v->bufsize){
		v->p = erealloc(v->p, v->bufsize * 2, v->bufsize);
		v->bufsize *= 2;
		v->vsize *= 2;
	}
	memcpy((char*)v->p+off, u, v->elsize);
}

VArray*
valloc(ulong n, int elsize)
{
	VArray *v;

	v = emalloc(sizeof *v);
	v->n = 0;
	v->elsize = elsize;
	v->vsize = MIN(n, VAdefsize);
	v->bufsize = v->vsize * elsize;
	v->p = emalloc(v->bufsize);
	return v;
}

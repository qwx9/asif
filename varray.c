#include <u.h>
#include <libc.h>
#include "asif.h"

enum{
	VAdefsize = 1024,
};

void
vfree(VAarray *v)
{
	free(v->p);
	free(v);
}

VArray*
vinsert(VArray *v, char *)
{
	int off;

	off = v->nelem * v->elsize;
	if(v->nelem++ >= v->bufsize){
		v->p = erealloc(v->p, v->bufsize * 2, v->bufsize);
		v->bufsize *= 2;
		v->vsize *= 2;
	}
	memcpy(v->p+off, u, v->elsize);
}

VArray*
valloc(ulong n, int elsize)
{
	VArray *v;

	v = emalloc(sizeof *p);
	v->nelem = 0;
	v->elsize = elsize;
	v->vsize = MIN(n, VAdefsize);
	v->bufsize = v->vsize * elsize;
	v->p = emalloc(v->bufsize);
	return v;
}

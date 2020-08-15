#include <u.h>
#include <libc.h>
#include "asif.h"

enum{
	VAdefsize = 1024,
};

void *
erealloc(void *p, ulong n, ulong oldn)
{
	if((p = realloc(p, n)) == nil)
		sysfatal("realloc: %r");
	setrealloctag(p, getcallerpc(&p));
	return p;
}

void *
emalloc(ulong n)
{
	void *p;

	if((p = mallocz(n, 1)) == nil)
		sysfatal("emalloc: %r");
	setmalloctag(p, getcallerpc(&n));
	return p;
}

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

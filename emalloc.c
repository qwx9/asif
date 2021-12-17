#include <u.h>
#include <libc.h>
#include "asif.h"

char *
estrdup(char *s)
{
	if((s = strdup(s)) == nil)
		sysfatal("estrdup: %r");
	setmalloctag(s, getcallerpc(&s));
	return s;
}

void *
erealloc(void *p, ulong n, ulong oldn)
{
	if((p = realloc(p, n)) == nil)
		sysfatal("realloc: %r");
	setrealloctag(p, getcallerpc(&p));
	if(n > oldn)
		memset((uchar *)p + oldn, 0, n - oldn);
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

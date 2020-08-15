#include <u.h>
#include <libc.h>
#include "asif.h"

/* naive exact string search of a word within a text */
VArray *
naivestrfind(String S, String W)
{
	int i, n;
	VArray *v;

	n = S.n - W.n + 1;
	if(n <= 0)
		return nil;
	v = valloc(n, sizeof(int));
	for(i=0; i<n; i++)
		if(strcmp(S.s+i, W.s) == 0)
			v = vinsert(v, (void*)&i);
	return v;
}

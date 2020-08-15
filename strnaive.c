#include <u.h>
#include <libc.h>
#include "asif.h"

VArray *
naivestrfind(String text, String word)
{
	int n;
	VArray *v;

	n = text.n - word.n + 1;
	if(n <= 0)
		return nil;
	v = valloc(n, sizeof int);
	for(i=0; i<n; i++)
		if(strcmp(text.s+i, word.s) == 0)
			v = vinsert(v, &i);
	return v;
}

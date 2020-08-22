#include <u.h>
#include <libc.h>
#include "asif.h"

/* morris-pratt exact string search of a word W within a text S with W preprocessing */

static int *
mpjumptab(String W)
{
	int i, j, *T;

	T = emalloc(W.n + 1);
	T[0] = -1;
	for(i=-1, j=0; j<W.n;){
		while(i > -1 && W.s[i] != W.s[j])
			i = T[i];
		T[++j] = ++i;
	}
	return T;
}

/* ordinary knuth-morris-pratt exact string search of a word W within a text S
 * with W preprocessing */

static int *
kmpjumptab(String W)
{
	int i, j, *T;

	T = emalloc(W.n + 1);
	T[0] = -1;
	for(i=-1, j=0; j<W.n;){
		while(i > -1 && W.s[i] != W.s[j])
			i = T[i];
		i++;
		j++;
		if(W.s[i] == W.s[j])
			T[j] = T[i];
		else
			T[j] = i;
	}
	return T;
}

static VArray *
search(String S, String W, int*(*tabfn)(String))
{
	int n, i, j, *T;
	VArray *v;

	n = S.n - W.n + 1;
	if(n <= 0)
		return nil;
	v = valloc(n, sizeof(int));
	T = tabfn(W);
	for(i=j=0; j<W.n;){
		while(i > -1 && W.s[i] != S.s[j])
			i = T[i];
		i++, j++;
		if(i >= W.n){
			n = j - i;
			vinsert(v, (void*)&n);
			i = T[i];
		}
	}
	free(T);
	return v;
}

VArray *
morrispratt(String S, String W)
{
	return search(S, W, mpjumptab);
}

VArray *
knuthmorrispratt(String S, String W)
{
	return search(S, W, kmpjumptab);
}

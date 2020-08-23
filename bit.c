#include <u.h>
#include <libc.h>
#include "asif.h"

/* find least significant set bit in 64bit integers */
int
lsb64(uvlong v)
{
	int c;

	c = 0;
	if((v & 0xffffffff) == 0){
		v >>= 32;
		c += 32;
	}
	if((v & 0xffff) == 0){
		v >>= 16;
		c += 16;
	}
	if((v & 0xff) == 0){
		v >>= 8;
		c += 8;
	}
	if((v & 0xf) == 0){
		v >>= 4;
		c += 4;
	}
	if((v & 3) == 0){
		v >>= 2;
		c += 2;
	}
	if((v & 1) == 0)
		c++;
	return c;
}

/* find first set bit in 64bit integers with lookup table */
static void
initffs(uchar *tab, int size)
{
	int i;

	tab[0] = 0;
	tab[1] = 0;
	for(i=2; i<size; i++)
		tab[i] = 1 + tab[i/2];
}
int
msb64(uvlong v)
{
	static uchar ffstab[256];
	int n;

	if(ffstab[nelem(ffstab)-1] == 0)
		initffs(ffstab, nelem(ffstab));
	if(n = v >> 56)
		return 56 + ffstab[n];
	else if(n = v >> 48)
		return 48 + ffstab[n];
	else if(n = v >> 40)
		return 40 + ffstab[n];
	else if(n = v >> 32)
		return 32 + ffstab[n];
	else if(n = v >> 24)
		return 24 + ffstab[n];
	else if(n = v >> 16)
		return 16 + ffstab[n];
	else if(n = v >> 8)
		return 8 + ffstab[n];
	else
		return ffstab[v];
}

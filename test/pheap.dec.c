#include <u.h>
#include <libc.h>
#include <bio.h>
#include "asif.h"
debuglevel = Logtrace;
enum{
	Nnodes = 10000,
};

void
main(int argc, char **argv)
{
	double v;
	char *suff, name[128];
	Pairheap *queue, *a, **t, **p;
	Biobuf *bf, *ubf;

	ARGBEGIN{
	}ARGEND
	suff = *argv == nil ? "" : *argv;
	t = emalloc(Nnodes * sizeof *t);
	snprint(name, sizeof name, "ph.unsorted%s", suff);
	if((ubf = Bopen(name, OWRITE)) == nil)
		sysfatal("Bopen: %r");
	snprint(name, sizeof name, "ph.sorted%s", suff);
	if((bf = Bopen(name, OWRITE)) == nil)
		sysfatal("Bopen: %r");
	queue = nil;
	srand(time(nil));
	for(p=t; p<t+Nnodes; p++){
		v = frand() * 1e6;
		*p = pushqueue(v, nil, &queue);
		if(frand() < 0.5){
			a = p == t ? *p : t[nrand(p-t)];
			decreasekey(a, frand() * 1e2, &queue);
		}
	}
	for(p=t; p<t+Nnodes; p++)
		Bprint(ubf, "%.2f\n", (*p)->n);
	Bterm(ubf);
	while((a = popqueue(&queue)) != nil)
		Bprint(bf, "%.2f\n", a->n);
	Bterm(bf);
	exits(nil);
}

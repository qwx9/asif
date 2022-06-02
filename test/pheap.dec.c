#include <u.h>
#include <libc.h>
#include "asif.h"
debuglevel = Logtrace;
enum{
	Nnodes = 10000,
};

void main(void) {
	Pairheap *queue, *a, **t, **p;
	int fd, i;
	double v;

	t = emalloc(Nnodes * sizeof *t);
	if((fd = create("res", OWRITE, 0644)) < 0)
		sysfatal("create: %r");
	queue = nil;
	srand(time(nil));
	for(p=t; p<t+Nnodes; p++){
		v = frand() * 1e6;
		*p = pushqueue(v, nil, &queue);
		if(frand() < 0.5){
			a = t[nrand(p-t)];
			decreasekey(a, a->n-frand()*1e2, &queue);
		}
	}
	for(p=t; p<t+Nnodes; p++)
		fprint(fd, "%.2f\n", (*p)->n);
	close(fd);
	while((a = popqueue(&queue)) != nil)
		print("%.2f\n", a->n);
	exits(nil);
}

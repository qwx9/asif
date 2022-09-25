#include <u.h>
#include <libc.h>
#include <bio.h>
#include "asif.h"
enum{
	Nnodes = 10000,
	Max = Nnodes,
};

int
cmp(void *u, void *v)
{
	double n, m;
	Pairheap **a, **b;

	a = u;
	b = v;
	n = (*a)->n;
	m = (*b)->n;
	if(n < m)
		return -1;
	else if(n > m)
		return 1;
	return 0;
}

void
main(void)
{
	int i;
	Pairheap *p, *queue, *nodes[Nnodes];

	srand(time(nil));
	queue = nil;
	for(i=0; i<Nnodes; i++){
		if(i > 1 && frand() < 0.6){
			p = popqueue(&queue);
			dprint(Logdebug, "pop %#p nodes[0] %#p\n", p, nodes[0]);
			assert(p == nodes[0]);
			free(p);
			nodes[0] = pushqueue(frand() * Max, nil, &queue);
			dprint(Logdebug, "push %d %#p %f\n", 0, nodes[0], nodes[0]->n);
		}
		if(i > 1 && frand() < 0.2){
			p = nodes[nrand(i)];
			decreasekey(p, frand() * (Max / 10), &queue);
		}
		nodes[i] = pushqueue(frand() * Max, nil, &queue);
		dprint(Logdebug, "push %d %#p %f\n", i, nodes[i], nodes[i]->n);
		qsort(nodes, i+1, sizeof(Pairheap*), cmp);
		for(int j=0; j<=i; j++)
			dprint(Logdebug, "sorted %d %#p %f\n", j, nodes[j], nodes[j]->n);
	}
	for(i=0; i<Nnodes; i++)
		dprint(Logdebug, "%f\n", nodes[i]->n);
	dprint(Logdebug, "end\n");
	i = 0;
	while((p = popqueue(&queue)) != nil){
		dprint(Logdebug, "%f\n", p->n);
		assert(p == nodes[i]);
		i++;
	}
	exits(nil);
}

#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

static Node *
dijkstra(Node *a, Node *b)
{
	double g, Δg;
	Node *x, *s, **sl;
	Pairheap *queue, *pn;

	assert(a != nil && b != nil);
	assert(a != b);
	queue = nil;
	x = a;
	a->pq = pushqueue(0, a, &queue);
	while((pn = popqueue(&queue)) != nil){
		x = pn->aux;
		free(pn);
		if(x == b)
			break;
		x->closed = 1;
		if((sl = successorfn(x)) == nil)
			sysfatal("dijkstra: %r");
		for(s=*sl++; s!=nil; s=*sl++){
			if(s->closed)
				continue;
			assert(!isblocked(s));
			g = x->g + s->Δg;
			Δg = s->g - g;
			if(!s->open){
				s->from = x;
				s->open = 1;
				s->g = g;
				s->pq = pushqueue(s->g, s, &queue);
			}else if(Δg > 0){
				s->from = x;
				s->g -= Δg;
				decreasekey(s->pq, Δg, &queue);
				assert(s->g >= 0);
			}
		}
	}
	nukequeue(&queue);
	return x;
}

void
threadmain(int argc, char **argv)
{
	init(argc, argv);
	initgrid(dijkstra);
	evloop();
}

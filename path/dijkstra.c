#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

/* currently uniform cost search and no reprioritizing (decrease-key operation) */

/* explore in all directions at the same time */
static double
movecost(int Δx, int Δy)
{
	return Δx != 0 && Δy != 0 ? SQRT2 : 1.0;
}

static Node *
dijkstra(Node *a, Node *b)
{
	double g, Δg;
	Node *x, *s, **sl;
	Pairheap *queue, *pn;

	assert(a != nil && b != nil);
	assert(a != b);
	queue = nil;
	a->pq = pushqueue(0, a, &queue);
	x = a;
	while((pn = popqueue(&queue)) != nil){
		x = pn->aux;
		free(pn);
		if(x == b)
			break;
		x->closed = 1;
		dprint(Logtrace, "dijkstrdijkstra: closed [%#p,%P] h %.4f g %.4f\n",
			x, n2p(x), x->h, x->g);
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
				dprint(Logtrace, "dijkstra: opened [%#p,%P] h %.4f g %.4f f %.4f\n",
					s, n2p(s), s->h, s->g, s->h + s->g);
				s->pq = pushqueue(s->g, s, &queue);
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
	initgrid(dijkstra, movecost);
	evloop();
}

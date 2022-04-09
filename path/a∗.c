#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

/* FIXME: nope, non-optimal paths + too many nodes opened, again
 * (dijsktra works fine) */

/* slightly penalize diagonal movement for nicer-looking paths; cf.:
 * https://www.redbloblgames.com/pathfinding/a-star/implementation.html
 * one addition: make cost function to increase at a slower rate to
 * resolve tie-breakers in favor of closer nodes, otherwise we will
 * explore all nodes in the rectangle between the two points; does
 * NOT work with 4-way movement and manhattan distance for some reason */
static double
movecost(int Δx, int Δy)
{
	return Δx != 0 && Δy != 0 ? 1.001 : 1.0;
}

static Node *
a∗(Node *a, Node *b)
{
	double g, Δg;
	Node *x, *s, **sl;
	Pairheap *queue, *pn;

	assert(a != nil && b != nil);
	assert(a != b);
	queue = nil;
	a->pq = pushqueue(distfn(a, b), a, &queue);
	x = a;
	while((pn = popqueue(&queue)) != nil){
		x = pn->aux;
		free(pn);
		if(x == b)
			break;
		x->closed = 1;
		dprint(Logtrace, "a∗: closed [%#p,%P] h %.4f g %.4f\n",
			x, n2p(x), x->h, x->g);
		if((sl = successorfn(x)) == nil)
			sysfatal("a∗: %r");
		for(s=*sl++; s!=nil; s=*sl++){
			if(s->closed)
				continue;
			assert(!isblocked(s));
			g = x->g + s->Δg;
			Δg = s->g - g;
			if(!s->open){
				s->from = x;
				s->open = 1;
				s->h = distfn(s, b);
				s->g = g;
				dprint(Logtrace, "a∗: opened [%#p,%P] h %.4f g %.4f f %.4f\n",
					s, n2p(s), s->h, s->g, s->h + s->g);
				s->pq = pushqueue(s->g + s->h, s, &queue);
			}else if(Δg > 0){
				dprint(Logtrace, "a∗: decrease [%#p,%P] h %.4f g %.4f Δg %.4f → f %.4f\n",
					s, n2p(s), s->h, s->g, Δg, s->h + s->g - Δg);
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
	initgrid(a∗, movecost);
	evloop();
}

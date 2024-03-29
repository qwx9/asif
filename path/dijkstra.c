#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "asif.h"
#include "graph.h"
#include "path.h"

/* currently uniform cost search and no reprioritizing (decrease-key operation) */

typedef Vertex Point;

typedef struct PNode PNode;
struct PNode{
	Node *n;
	double g;
	double Δg;
	Pairheap *pq;
};

static Zpool *zpool;

static void
backtrack(Node *a, Node *b)
{
	Node *n;
	PNode *p;

	for(n=b; n!=a; n=n->from){
		p = n->aux;
		stats.cost += p->g;
		stats.steps++;
		n->from->to = n;
	}
}

static Node **
successors(Node *u)
{
	Node **vl, **vp, *n;
	PNode *pu;

	vl = expand(u);
	for(vp=vl; (n=*vp)!=nil; vp++)
		if(n->aux == nil){
			pu = n->aux = zalloc(zpool);
			pu->n = n;
		}
	return vl;
}

static int
dijkstra(Node *a, Node *b)
{
	double g, Δg;
	PNode *pu, *pv;
	Node *u, *v, **vl;
	Pairheap *queue, *pq;

	queue = nil;
	u = a;
	pu = zalloc(zpool);
	pu->n = u;
	u->aux = pu;
	pu->pq = pushqueue(distfn(a, b), pu, &queue);
	while((pq = popqueue(&queue)) != nil){
		pu = pq->aux;
		u = pu->n;
		free(pq);
		if(u == b)
			break;
		u->closed = 1;
		stats.closed++;
		dprint(Logtrace, "dijkstra: closed [%#p,%P] g %.4f\n",
			u, n2p(u), pu->g);
		if((vl = successors(u)) == nil)
			sysfatal("a∗: %r");
		for(v=*vl++; v!=nil; v=*vl++){
			pv = v->aux;
			stats.opened++;
			if(v->closed)
				continue;
			g = pu->g + unitmovecost(u, v);
			Δg = pv->g - g;
			assert(floor(Δg) <= 0.0);
			if(!v->open){
				v->from = u;
				v->open = 1;
				stats.expanded++;
				pv->g = g;
				dprint(Logtrace, "dijkstra: expanded [%#p,%P] g %.4f\n",
					v, n2p(v), pv->g);
				pv->pq = pushqueue(pv->g, pv, &queue);
			}
		}
	}
	nukequeue(&queue);
	if(u != b)
		return -1;
	return 0;
}

int
dijkstrafindpath(Node *a, Node *b)
{
	int r;

	assert(a != nil && b != nil && a != b);
	clearpath();
	if(zpool == nil)
		zpool = znew(sizeof(PNode));
	dprint(Logdebug, "grid::dijkstrafindpath: dijkstra from [%#p,%P] to [%#p,%P]\n",
		a, n2p(a), b, n2p(b));
	stats.dist = distfn(a, b);
	if((r = dijkstra(a, b)) < 0)
		dprint(Logdebug, "grid::dijkstrafindpath: failed to find a path\n");
	else
		backtrack(a, b);
	znuke(zpool);
	if(r >= 0)
		dprintpath(a, b);
	return r;
}

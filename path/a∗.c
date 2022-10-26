#include <u.h>
#include <libc.h>
#include <draw.h>
#include "asif.h"
#include "graph.h"
#include "path.h"

typedef Vertex Point;

typedef struct PNode PNode;
struct PNode{
	Node *n;
	double g;
	double h;
	double Δg;
	Pairheap *pq;
};

static Zpool *zpool;

static void
backtrack(Node *a, Node *b)
{
	Node *u, *v;
	PNode *p;

	for(u=b; u!=a; u=v){
		p = u->aux;
		stats.cost += p->g;
		stats.steps++;
		v = u->from;
		v->to = u;
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
a∗(Node *a, Node *b)
{
	double g, Δg;
	Node *u, *v, **vl;
	PNode *pu, *pv;
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
		dprint(Logtrace, "a∗: closed [%#p,%P] h %.4f g %.4f\n",
			u, n2p(u), pu->h, pu->g);
		if((vl = successors(u)) == nil)
			sysfatal("a∗: %r");
		for(v=*vl++; v!=nil; v=*vl++){
			pv = v->aux;
			stats.opened++;
			if(v->closed)
				continue;
			g = pu->g + unitmovecost(u, v);
			Δg = pv->g - g;
			if(!v->open){
				v->from = u;
				v->open = 1;
				stats.expanded++;
				pv->h = distfn(v, b);
				pv->g = g;
				dprint(Logtrace, "a∗: expanded [%#p,%P] h %.4f g %.4f f %.4f\n",
					v, n2p(v), pv->h, pv->g, pv->h + pv->g);
				pv->pq = pushqueue(pv->g + pv->h, pv, &queue);
			}else if(Δg > 0){
				stats.updated++;
				dprint(Logtrace, "a∗: decrease [%#p,%P] h %.4f g %.4f Δg %.4f → f %.4f\n",
					v, n2p(v), pv->h, pv->g, Δg, pv->h + pv->g - Δg);
				v->from = u;
				pv->g -= Δg;
				decreasekey(pv->pq, Δg, &queue);
				assert(pv->g >= 0);
			}
		}
	}
	nukequeue(&queue);
	if(u != b)
		return -1;
	return 0;
}

int
a∗findpath(Node *a, Node *b)
{
	int r;

	assert(a != nil && b != nil && a != b);
	clearpath();
	if(zpool == nil)
		zpool = znew(sizeof(PNode));
	dprint(Logdebug, "grid::a∗findpath: a∗ from [%#p,%P] to [%#p,%P]\n",
		a, n2p(a), b, n2p(b));
	stats.dist = distfn(a, b);
	if((r = a∗(a, b)) < 0)
		dprint(Logdebug, "grid::a∗findpath: failed to find a path\n");
	else
		backtrack(a, b);
	znuke(zpool);
	if(r >= 0)
		dprintpath(a, b);
	return r;
}

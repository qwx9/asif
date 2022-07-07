#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "asif.h"
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

static PNode**	(*successorfn)(Node*);
static Zpool *zpool;

static void
cleanup(Pairheap **queue)
{
	Pairheap *p;

	while((p = popqueue(queue)) != nil){
		zfree(p->aux);
		free(p);
	}
}

static void
backtrack(Node *a, Node *b)
{
	Node *n;

	for(n=b; n!=a; n=n->from)
		n->from->to = n;
}

/* slightly penalize diagonal movement for nicer-looking paths; cf.:
 * https://www.redbloblgames.com/pathfinding/a-star/implementation.html
 * one addition: make cost function to increase at a slower rate to
 * resolve tie-breakers in favor of closer nodes, otherwise we will
 * explore all nodes in the rectangle between the two points */
static double
movecost(int Δx, int Δy)
{
	return Δx != 0 && Δy != 0 ? 1.001 : 1.0;
}

static PNode **
successors8(Node *nu)
{
	static PNode *suc[8+1];
	static dtab[2*(nelem(suc)-1)]={
		1,0, 0,-1, -1,0, 0,1,
		-1,-1, -1,1, 1,-1, 1,1,
	};
	int i;
	Node *nv;
	PNode *v, **vp;
	Point p;
	Rectangle r;

	memset(suc, 0, sizeof suc);
	p = n2p(nu);
	r = Rect(0, 0, gridwidth, gridheight);
	for(i=0, vp=suc; i<nelem(dtab); i+=2){
		if(!ptinrect(addpt(p, Pt(dtab[i], dtab[i+1])), r))
			continue;
		nv = nu + dtab[i+1] * gridwidth + dtab[i];
		assert(nv >= grid && nv < grid + gridwidth * gridheight);
		if(isblocked(nv))
			continue;
		v = zalloc(zpool);
		v->n = nv;
		v->Δg = movecost(dtab[i], dtab[i+1]);
		*vp++ = v;
	}
	return suc;
}

static PNode **
successors4(Node *nu)
{
	static PNode *suc[4+1];
	static int dtab[2*(nelem(suc)-1)]={
		1,0, -1,0, 0,-1, 0,1,
	}, rdtab[nelem(dtab)]={
		0,1, 0,-1, -1,0, 1,0,
	};
	int i, *t;
	Node *nv;
	PNode *v, **vp;
	Point p;
	Rectangle r;

	memset(suc, 0, sizeof suc);
	p = n2p(nu);
	r = Rect(0, 0, gridwidth, gridheight);
	/* path straightening; cf.:
	 * https://www.redbloblgames.com/pathfinding/a-star/implementation.html */
	t = (p.x + p.y) % 2 == 0 ? rdtab : dtab;
	for(i=0, vp=suc; i<nelem(dtab); i+=2){
		if(!ptinrect(addpt(p, Pt(t[i], t[i+1])), r))
			continue;
		nv = nu + t[i+1] * gridwidth + t[i];
		assert(nv >= grid && nv < grid + gridwidth * gridheight);
		if(isblocked(nv))
			continue;
		v = zalloc(zpool);
		v->n = nv;
		v->Δg = movecost(t[i], t[i+1]);
		*vp++ = v;
	}
	return suc;
}

static int
dijkstra(Node *a, Node *b)
{
	double g, Δg;
	PNode *u, *v, **vl;
	Node *nu, *nv;
	Pairheap *queue, *pn;

	queue = nil;
	nu = a;
	u = zalloc(zpool);
	u->n = a;
	u->pq = pushqueue(distfn(a, b), u, &queue);
	while((pn = popqueue(&queue)) != nil){
		u = pn->aux;
		nu = u->n;
		free(pn);
		if(nu == b)
			break;
		nu->closed = 1;
		dprint(Logtrace, "dijkstra: closed [%#p,%P] g %.4f\n",
			u, n2p(nu), u->g);
		if((vl = successorfn(nu)) == nil)
			sysfatal("a∗: %r");
		for(v=*vl++; v!=nil; v=*vl++){
			nv = v->n;
			if(nv->closed)
				continue;
			g = u->g + v->Δg;
			Δg = v->g - g;
			if(!nv->open){
				nv->from = nu;
				nv->open = 1;
				v->g = g;
				dprint(Logtrace, "dijkstra: opened [%#p,%P] g %.4f\n",
					v, n2p(nv), v->g);
				v->pq = pushqueue(v->g, v, &queue);
			}
			assert(Δg <= 0);
		}
	}
	cleanup(&queue);
	if(nu != b)
		return -1;
	backtrack(a, b);
	return 0;
}

int
dijkstrafindpath(Node *a, Node *b)
{
	assert(a != nil && b != nil && a != b);
	clearpath();
	if(zpool == nil)
		zpool = znew(sizeof(PNode));
	successorfn = movemode == Move8 ? successors8 : successors4;
	dprint(Logdebug, "grid::dijkstrafindpath: dijkstra from [%#p,%P] to [%#p,%P]\n",
		a, n2p(a), b, n2p(b));
	if(dijkstra(a, b) < 0){
		dprint(Logdebug, "grid::dijkstrafindpath: failed to find a path\n");
		return -1;
	}
	return 0;
}

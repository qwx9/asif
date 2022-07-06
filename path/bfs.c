#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "asif.h"
#include "path.h"

typedef Vertex Point;

typedef Node PNode;
static PNode**	(*successorfn)(Node*);

static void
cleanup(void)
{
}

static double
movecost(int Δx, int Δy)
{
	return Δx != 0 && Δy != 0 ? 1.001 : 1.0;
}

static PNode **
successors8(Node *u)
{
	static PNode *suc[8+1];
	static dtab[2*(nelem(suc)-1)]={
		1,0, 0,-1, -1,0, 0,1,
		-1,-1, -1,1, 1,-1, 1,1,
	};
	int i;
	Node *v, **vp;
	Point p;
	Rectangle r;

	memset(suc, 0, sizeof suc);
	p = n2p(u);
	r = Rect(0, 0, gridwidth, gridheight);
	for(i=0, vp=suc; i<nelem(dtab); i+=2){
		if(!ptinrect(addpt(p, Pt(dtab[i], dtab[i+1])), r))
			continue;
		v = u + dtab[i+1] * gridwidth + dtab[i];
		assert(v >= grid && v < grid + gridwidth * gridheight);
		if(isblocked(v))
			continue;
		*vp++ = v;
	}
	return suc;
}

static PNode **
successors4(Node *u)
{
	static Node *suc[4+1];
	static int dtab[2*(nelem(suc)-1)]={
		1,0, -1,0, 0,-1, 0,1,
	}, rdtab[nelem(dtab)]={
		0,1, 0,-1, -1,0, 1,0,
	};
	int i, *t;
	Node *v, **vp;
	Point p;
	Rectangle r;

	memset(suc, 0, sizeof suc);
	p = n2p(u);
	r = Rect(0, 0, gridwidth, gridheight);
	/* path straightening; cf.:
	 * https://www.redbloblgames.com/pathfinding/a-star/implementation.html */
	t = (p.x + p.y) % 2 == 0 ? rdtab : dtab;
	for(i=0, vp=suc; i<nelem(dtab); i+=2){
		if(!ptinrect(addpt(p, Pt(t[i], t[i+1])), r))
			continue;
		v = u + t[i+1] * gridwidth + t[i];
		assert(v >= grid && v < grid + gridwidth * gridheight);
		if(isblocked(v))
			continue;
		*vp++ = v;
	}
	return suc;
}

static int
bfs(Node *a, Node *b)
{
	Vector *front;
	Node *u, *v, **vl;

	assert(a != nil && b != nil);
	assert(a != b);
	front = vec(sizeof u);
	u = a;
	vecpush(front, &u);
	while(front->len > 0){
		u = *((PNode **)vechpop(front));
		if(u == b)
			break;
		u->closed = 1;
		if((vl = successorfn(u)) == nil)
			sysfatal("bfs: %r");
		for(v=*vl++; v!=nil; v=*vl++){
			if(v->open)
				continue;
			v->from = u;
			vecpush(front, &v);
			v->open = 1;
		}
	}
	vecfree(front);
	return 0;
}

int
bfsfindpath(Node *a, Node *b)
{
	assert(a != nil && b != nil && a != b);
	clearpath();
	successorfn = movemode == Move8 ? successors8 : successors4;
	dprint(Logdebug, "grid::bfsfindpath: bfs from [%#p,%P] to [%#p,%P]\n",
		a, n2p(a), b, n2p(b));
	if(bfs(a, b) < 0){
		dprint(Logdebug, "grid::bfsfindpath: failed to find a path\n");
		return -1;
	}
	return 0;
}

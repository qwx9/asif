#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

extern int	(*mousefn)(Node*, Mouse, Node*);
extern int	(*keyfn)(Rune);

double	(*distfn)(Node*, Node*);
Node**	(*successorfn)(Node*);

Node *start, *goal;

static Node*	(*pathfn)(Node*, Node*);
static double	(*costfn)(int, int);

static void
backtrack(void)
{
	Node *n;

	assert(goal != start);
	for(n=goal; n!=start; n=n->from)
		n->from->to = n;
}

static Node **
successors8(Node *n)
{
	static Node *suc[8+1];
	static dtab[2*(nelem(suc)-1)]={
		1,0, 0,-1, -1,0, 0,1,
		-1,-1, -1,1, 1,-1, 1,1,
	};
	int i;
	Node *s, **np;
	Point p;
	Rectangle r;

	memset(suc, 0, sizeof suc);
	p = n2p(n);
	r = Rect(0, 0, mapwidth, mapheight);
	for(i=0, np=suc; i<nelem(dtab); i+=2){
		if(!ptinrect(addpt(p, Pt(dtab[i], dtab[i+1])), r))
			continue;
		s = n + dtab[i+1] * mapwidth + dtab[i];
		assert(s >= map && s < map + mapwidth * mapheight);
		if(isblocked(s))
			continue;
		s->Δg = costfn != nil ? costfn(dtab[i], dtab[i+1]) : 0;
		*np++ = s;
	}
	return suc;
}

static Node **
successors4(Node *n)
{
	static Node *suc[4+1];
	static int dtab[2*(nelem(suc)-1)]={
		1,0, -1,0, 0,-1, 0,1,
	}, rdtab[nelem(dtab)]={
		0,1, 0,-1, -1,0, 1,0,
	};
	int i, *t;
	Node *s, **np;
	Point p;
	Rectangle r;

	memset(suc, 0, sizeof suc);
	p = n2p(n);
	r = Rect(0, 0, mapwidth, mapheight);
	/* path straightening; cf.:
	 * https://www.redbloblgames.com/pathfinding/a-star/implementation.html */
	t = (p.x + p.y) % 2 == 0 ? rdtab : dtab;
	for(i=0, np=suc; i<nelem(dtab); i+=2){
		if(!ptinrect(addpt(p, Pt(t[i], t[i+1])), r))
			continue;
		s = n + t[i+1] * mapwidth + t[i];
		assert(s >= map && s < map + mapwidth * mapheight);
		if(isblocked(s))
			continue;
		s->Δg = costfn != nil ? costfn(t[i], t[i+1]) : 0;
		*np++ = s;
	}
	return suc;
}

static int
findpath(void)
{
	if(start == nil || goal == nil){
		werrstr("findpath: start and goal not undefined");
		return -1;
	}
	resetmap();
	dprint(Logdebug, "grid::findpath: findpath from [%#p,%P] to [%#p,%P]\n",
		start, n2p(start), goal, n2p(goal));
	if(pathfn(start, goal) != goal){
		werrstr("findpath: failed");
		return -1;
	}
	backtrack();
	return 0;
}

static int
grmouse(Node *n, Mouse m, Node *old)
{
	switch(m.buttons & 7){
	case 1: if(goal != n && !n->blocked) start = n; break;
	case 2: if(start != n && !n->blocked) goal = n; break;
	case 4: if(old == nil || n->blocked ^ old->blocked) n->blocked ^= 1; return 0;
	}
	if(start != nil && goal != nil)
		return findpath();
	return 0;
}

static int
grkey(Rune)
{
	return 0;
}

void
initgrid(Node* (*f)(Node*, Node*), double (*cost)(int, int))
{
	if(fourdir){
		distfn = manhdist;
		successorfn = successors4;
	}else{
		distfn = octdist;
		successorfn = successors8;
	}
	mousefn = grmouse;
	keyfn = grkey;
	pathfn = f;
	costfn = cost;
}

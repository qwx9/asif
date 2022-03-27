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
Node*	(*pathfn)(Node*, Node*);

Node *start, *goal;

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
		0,-1, 1,0, 0,1, -1,0,
		1,-1, 1,1, -1,1, -1,-1,
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
		s->Δg = 1;
		//s->Δg = dtab[i] != 0 && dtab[i+1] != 0 ? SQRT2 : 1;
		*np++ = s;
	}
	return suc;
}

static Node **
successors4(Node *n)
{
	static Node *suc[4+1];
	static dtab[2*(nelem(suc)-1)]={
		0,-1, -1,0, 1,0, 0,1,
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
		s->Δg = 1;
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
initgrid(Node* (*f)(Node*, Node*))
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
}

#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

Node *start, *goal;

static double	(*distfn)(Node*, Node*);
static Node**	(*successorfn)(Node*);

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

static Node *
a∗(Node *a, Node *b)
{
	double g, Δg;
	Node *x, *s, **sl;
	Pairheap *queue, *pn;

	assert(a != nil && b != nil);
	assert(a != b);
	queue = nil;
	x = a;
	a->pq = pushqueue(distfn(a, b), a, &queue);
	while((pn = popqueue(&queue)) != nil){
		x = pn->aux;
		free(pn);
		if(x == b)
			break;
		x->closed = 1;
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
				s->pq = pushqueue(s->g + s->h, s, &queue);
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

static int
findpath(void)
{
	if(start == nil || goal == nil)
		return -1;
	resetmap();
	if(a∗(start, goal) != goal)
		return -1;
	backtrack();
	return 0;
}

int
mouseinput(Node *n, Mouse m, Node *old)
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

int
keyinput(Rune)
{
	return 0;
}

void
threadmain(int argc, char **argv)
{
	init(argc, argv);
	if(fourdir){
		distfn = manhdist;
		successorfn = successors4;
	}else{
		distfn = octdist;
		successorfn = successors8;
	}
	evloop();
}

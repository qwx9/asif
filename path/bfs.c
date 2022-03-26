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
		s->Δg = dtab[i] != 0 && dtab[i+1] != 0 ? SQRT2 : 1;
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
bfs(Node *a, Node *b)
{
	Vector *v;
	Node *x, *s, **sl;

	assert(a != nil && b != nil);
	assert(a != b);
	v = vec(sizeof a);
	x = a;
	vecpush(v, &x);
	while(v->len > 0){
		x = *((Node **)vechpop(v));
		if(x == b)
			break;
		x->closed = 1;
		if((sl = successorfn(x)) == nil)
			sysfatal("bfs: %r");
		for(s=*sl++; s!=nil; s=*sl++){
			if(s->open)
				continue;
			s->from = x;
			vecpush(v, &s);
			s->open = 1;
		}
	}
	vecfree(v);
	return x;
}

static int
findpath(void)
{
	if(start == nil || goal == nil){
		werrstr("findpath: start and/or goal undefined");
		return -1;
	}
	resetmap();
	if(bfs(start, goal) != goal){
		werrstr("findpath: failed");
		return -1;
	}
	backtrack();
	return 0;
}

int
mouseinput(Node *n, Mouse m)
{
	switch(m.buttons & 7){
	case 1: if(goal != n) start = n; return findpath();
	case 2: if(start != n) goal = n; return findpath();
	case 4: n->blocked ^= 1; break;
	}
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
	if(fourdir)
		successorfn = successors4;
	else
		successorfn = successors8;
	evloop();
}

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

static void
backtrack(void)
{
	Node *n;

	assert(goal != start);
	for(n=goal; n!=start; n=n->from)
		n->from->to = n;
}

static Node **
successors(Node *n)
{
	static Node *suc[8+1];
	static dtab[2*(nelem(suc)-1)]={
		-1,-1, 0,-1, 1,-1,
		-1,0, 1,0,
		-1,1, 0,1, 1,1,
	};
	int i;
	Node *s, **np;

	memset(suc, 0, sizeof suc);
	for(i=0, np=suc; i<nelem(dtab); i+=2){
		s = n + dtab[i+1] * mapwidth + dtab[i];
		if(s < map || s >= map + mapwidth * mapheight)
			continue;
		if(isblocked(s))
			continue;
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
	a->pq = pushqueue(octdist(a, b), a, &queue);
	while((pn = popqueue(&queue)) != nil){
		x = pn->aux;
		free(pn);
		if(x == b)
			break;
		x->closed = 1;
		if((sl = successors(x)) == nil)
			sysfatal("a∗: %r");
		for(s=*sl++; s!=nil; s=*sl++){
			if(s->closed)
				continue;
			if(isblocked(s))
				continue;
			g = x->g + 1;	// recheck if always 1 or sqrt2
			Δg = s->g - g;
			if(!s->open){
				s->from = x;
				s->open = 1;
				s->h = octdist(s, b);
				s->g = g;
				pushqueue(s->g, s, &queue);
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
mouseinput(Node *n, Mouse m)
{
	switch(m.buttons & 7){
	case 1: goal = n; return findpath();
	case 2: start = n; return findpath();
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
	evloop();
}

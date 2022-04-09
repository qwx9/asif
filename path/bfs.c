#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

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

void
threadmain(int argc, char **argv)
{
	init(argc, argv);
	initgrid(bfs, nil);
	evloop();
}

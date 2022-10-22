#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "asif.h"
#include "graph.h"
#include "path.h"

typedef Vertex Point;

static void
backtrack(Node *a, Node *b)
{
	Node *n;

	for(n=b; n!=a; n=n->from){
		stats.cost++;
		stats.steps++;
		n->from->to = n;
	}
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
		u = *((Node **)vechpop(front));
		if(u == b)
			break;
		stats.closed++;
		u->closed = 1;
		if((vl = expand(u)) == nil)
			sysfatal("bfs: %r");
		for(v=*vl++; v!=nil; v=*vl++){
			stats.touched++;
			if(v->open)
				continue;
			stats.opened++;
			v->from = u;
			vecpush(front, &v);
			v->open = 1;
		}
	}
	vecfree(front);
	if(u != b)
		return -1;
	backtrack(a, b);
	return 0;
}

int
bfsfindpath(Node *a, Node *b)
{
	assert(a != nil && b != nil && a != b);
	clearpath();
	dprint(Logdebug, "grid::bfsfindpath: bfs from [%#p,%P] to [%#p,%P]\n",
		a, n2p(a), b, n2p(b));
	stats.dist = distfn(a, b);
	if(bfs(a, b) < 0){
		dprint(Logdebug, "grid::bfsfindpath: failed to find a path\n");
		return -1;
	}
	dprintpath(a, b);
	return 0;
}

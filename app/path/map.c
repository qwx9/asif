#include <u.h>
#include <libc.h>
#include "asif.h"
#include "graph.h"
#include "path.h"
#include "dat.h"
#include "fns.h"

int movemode;
int curscen;

static int	(*pathfn)(Node*, Node*);

Vertex
n2s(Node *n)
{
	Vertex v;

	v = n2p(n);
	v.x = v.x * nodesz + 1;
	v.y = v.y * nodesz + 1;
	return v;
}

int
trypath(Node *a, Node *b)
{
	if(a == nil || b == nil)
		return -1;
	dprint(Logdebug, "path::trypath: from %N to %N: ", start, goal);
	if(pathfn(a, b) < 0){
		dprint(Logdebug, "failed: %r\n");
		return -1;
	}
	dprint(Logdebug, "success\n");
	return 0;
}

int
showscen(int id)
{
	Sim *sp;

	assert(id >= 0 && id < sims->n);
	curscen = id;
	sp = sims->p;
	sp += id;
	start = p2n(sp->start);
	goal = p2n(sp->goal);
	if(trypath(start, goal) < 0){
		errmsg("failed: %r");
		return -1;
	}
	return 0;
}

int
reloadscen(void)
{
	cleargrid();
	if(mapfile == nil)
		return 0;
	if(readmap(mapfile) < 0)
		return -1;
	return showscen(curscen);
}

void
savepath(Sim *sp)
{
	Node *n;
	Vertex v;

	if(sp->path == nil)
		sp->path = valloc(sp->steps, sizeof(Vertex));
	else
		vnuke(sp->path);
	for(n=start; n!=nil; n=n->to){
		v = n->Vertex;
		vinsert(sp->path, (char*)&v);
		if(n == goal)
			return;
	}
	/* FIXME: dumpcore function? */
	fprint(2, "savepath: malformed path\n");
	abort();
}

void
runallscens(void)
{
	Sim *sp, *se;

	for(sp=sims->p, se=sp+sims->n; sp<se; sp++){
		if(showscen(sp - (Sim *)sims->p) < 0)
			continue;
		sp->Prof = stats;
		savepath(sp);
	}
}

static int
setparm(Vertex *dim, int move, int alg, int dist)
{
	switch(move){
	case Move8: /* wet floor */
	case Move4: movemode = move; break;
	case -1:
		movemode = Move8;
		dprint(Logdebug, "set by default: 8-direction movement\n");
		break;
	default: sysfatal("setparm: unknown move mode %d", move);
	}
	switch(alg){
	case Pa∗: pathfn = a∗findpath; break;
	case Pbfs: pathfn = bfsfindpath; break;
	case Pdijkstra: pathfn = dijkstrafindpath; break;
	case -1:
		pathfn = a∗findpath;
		dprint(Logdebug, "set by default: unmodified A∗ algorithm\n");
		break;
	default: sysfatal("setparm: unknown algo type %d", alg);
	}
	switch(dist){
	case Deuclid: distfn = eucdist; break;
	case Dmanhattan: distfn = manhdist; break;
	case Doctile: distfn = octdist; break;
	case Dchebyshev: distfn = chebdist; break;
	case -1:
		distfn = chebdist;
		dprint(Logdebug, "set by default: chebyshev distance\n");
		break;
	default: sysfatal("setparm: unknown distance function %d", dist);
	}
	if(dim->x == -1)
		dim->x = 64;
	if(dim->y == -1)
		dim->y = 64;
	if(dim->x <= 0 || dim->x >= 512 || dim->y <= 0 || dim->y >= 512)
		sysfatal("setparm: invalid map size, must be in ]0,512]");
	return 0;
}

int
initmap(char *map, Vertex dim, int move, int alg, int dist)
{
	mapfile = map;
	setparm(&dim, move, alg, dist);
	if(map != nil && readmaphdr(map, &dim) < 0)
		sysfatal("initmap: %r");
	initgrid(dim.x, dim.y);
	if(map != nil && readmap(map) < 0)
		sysfatal("initmap: %r");
	return 0;
}

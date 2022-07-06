#include <u.h>
#include <libc.h>
#include "asif.h"
#include "path.h"
#include "dat.h"
#include "fns.h"

int movemode;
int	(*pathfn)(Node*, Node*);

Vertex
n2s(Node *n)
{
	Vertex v;

	v = n2p(n);
	v.x = v.x * Nodesz + 1;
	v.y = v.y * Nodesz + 1;
	return v;
}

int
setparm(int mmode, int alg, int dist)
{
	switch(mmode){
	case Move8: /* wet floor */
	case Move4: movemode = mmode; break;
	default: sysfatal("setparm: unknown move mode %d", mmode);
	}
	switch(alg){
	case Pa∗: pathfn = a∗findpath; break;
	case Pbfs: pathfn = bfsfindpath; break;
	//case Pdijkstra: pathfn = dijkstrafindpath; break;
	default: sysfatal("setparm: unknown algo type %d", alg);
	}
	switch(dist){
	case Deuclid: distfn = eucdist; break;
	case Dmanhattan: distfn = manhdist; break;
	case Doctile: distfn = octdist; break;
	default: sysfatal("setparm: unknown distance function %d", dist);
	}
	clearmap();
	return 0;
}

void
clearmap(void)
{
	if(grid == nil)
		return;
	cleargrid();
	start = goal = nil;
}

void
initmap(int w, int h)
{
	initgrid(w, h);
}

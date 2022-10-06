#include <u.h>
#include <libc.h>
#include "asif.h"
#include "graph.h"
#include "path.h"

Node *grid;
int gridwidth, gridheight;
double	(*distfn)(Node*, Node*);

int doprof;
Prof stats;

double
eucdist(Node *a, Node *b)
{
	int dx, dy;

	dx = a->x - b->x;
	dy = a->y - b->y;
	return sqrt(dx *dx + dy *dy);
}

double
octdist(Node *a, Node *b)
{
	int dx, dy;

	dx = abs(a->x - b->x);
	dy = abs(a->y - b->y);
	return 1 * (dx + dy) + MIN(dx, dy) * (SQRT2 - 2 * 1);
}

double
manhdist(Node *a, Node *b)
{
	int dx, dy;

	dx = abs(a->x - b->x);
	dy = abs(a->y - b->y);
	return dx + dy;
}

void
toggleblocked(Node *n)
{
	n->blocked ^= 1;
}

/* slightly penalize diagonal movement for nicer-looking paths; cf.:
 * https://www.redbloblgames.com/pathfinding/a-star/implementation.html
 * one addition: make cost function to increase at a slower rate to
 * resolve tie-breakers in favor of closer nodes, otherwise we will
 * explore all nodes in the rectangle between the two points */
double
unitmovecost(Node *a, Node *b)
{
	Vertex Δ;

	Δ = ΔV(*b, *a);
	return Δ.x != 0 && Δ.y != 0 ? 1.001 : 1.0;
}

int
isblocked(Node *n)
{
	if(n < grid || n >= grid + gridwidth * gridheight){
		fprint(2, "isblocked: access beyond borders at %N\n", n);
		return 1;
	}
	return n->blocked;
}

Node **
expand4(Node *u)
{
	static Node *neigh[4+1];
	static Vertex dtab[]={
		{1,0}, {-1,0}, {0,-1}, {0,1},
	}, rdtab[nelem(dtab)]={
		{0,1}, {0,-1}, {-1,0}, {1,0},
	};
	int i;
	Node *v, **vl;
	Vertex p, p´, *dir;
	Vectangle r;

	memset(neigh, 0, sizeof neigh);
	p = n2p(u);
	r = V²(0, 0, gridwidth, gridheight);
	/* simple path straightening, cf.:
	 * https://www.redblobgames.com/pathfinding/a-star/implementation.html */
	dir = (p.x + p.y) % 2 == 0 ? rdtab : dtab;
	for(i=0, vl=neigh; i<nelem(dtab); i++){
		p´ = ∑V(p, dir[i]);
		if(!V∩V²(p´, r))
			continue;
		v = u + p´.y * gridwidth + p´.x;
		assert(v >= grid && v < grid + gridwidth * gridheight);
		if(!isblocked(v))
			*vl++ = v;
	}
	return neigh;
}

Node **
expand8(Node *u)
{
	static Node *neigh[8+1];
	/* same as for expand4, order tweaked for nicer paths */
	static Vertex dir[] = {
		{1,0}, {0,-1}, {-1,0}, {0,1},
		{-1,-1}, {-1,1}, {1,-1}, {1,1},
	};
	static dmask[] = {
		θ→, θ↑, θ←, θ↓,
		θ← | θ↑, θ← | θ↓, θ→ | θ↑, θ→ | θ↓
	};
	int i, open;
	Node *v, **vl;
	Vertex p, p´;
	Vectangle r;

	assert(u >= grid && u < grid + gridwidth * gridheight);
	memset(neigh, 0, sizeof neigh);
	p = n2p(u);
	r = V²(0, 0, gridwidth, gridheight);
	for(i=0, vl=neigh, open=0; i<nelem(dir); i++){
		p´ = ∑V(p, dir[i]);
		if(!V∩V²(p´, r)){
			if(i < 4)
				open |= dmask[i];
			dprint(Logtrace, "%N→%V out of bounds\n", u, p´);
			continue;
		}
		v = grid + p´.y * gridwidth + p´.x;
		if(isblocked(v)){
			if(i < 4)
				open |= dmask[i];
			continue;
		/* forbid corner cutting */
		}else if((open & dmask[i]) != 0){
			dprint(Logtrace, "%N→%N move disallowed, dir %#02ux blk %#02ux\n",
				u, v, dmask[i], open);
			continue;
		}
		*vl++ = v;
	}
	return neigh;
}

Node **
expand(Node *n)
{
	return movemode == Move8 ? expand8(n) : expand4(n);
}

void
dprintpath(Node *n, Node *goal)
{
	if(debuglevel < Logtrace)
		return;
	dprint(Logtrace, "path: ");
	while(n != goal){
		dprint(Logtrace, "%N ", n);
		n = n->to;
	}
}

void
clearpath(void)
{
	Node *n;

	if(grid == nil)
		return;
	for(n=grid; n<grid+gridwidth*gridheight; n++)
		memset(&n->PState, 0, sizeof n->PState);
	memset(&stats, 0, sizeof stats);
}

void
cleargrid(void)
{
	Node *n;

	if(grid == nil)
		return;
	for(n=grid; n<grid+gridwidth*gridheight; n++)
		memset(&n->State, 0, sizeof n->State);
}

Node *
p2n(Vertex p)
{
	return grid + p.y * gridwidth + p.x;
}

Vertex
n2p(Node *n)
{
	return (Vertex){(n - grid) % gridwidth, (n - grid) / gridheight};
}

int
Vfmt(Fmt *f)
{
	Vertex v;

	v = va_arg(f->args, Vertex);
	return fmtprint(f, "[%d %d]", v.x, v.y);
}

int
Nfmt(Fmt *f)
{
	Node *n;
	Vertex v;

	n = va_arg(f->args, Node*);
	v = n2p(n);
	return fmtprint(f, "[%#p %V]", n, v);
}

Node *
initgrid(int w, int h)
{
	int x, y;
	Node *n;

	grid = emalloc(w * h * sizeof *grid);
	for(n=grid, x=0, y=0; n<grid+w*h; n++){
		n->x = x;
		n->y = y;
		if(++x == w){
			x = 0;
			y++;
		}
	}
	gridwidth = w;
	gridheight = h;
	return grid;
}

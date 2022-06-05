#include <u.h>
#include <libc.h>
#include "asif.h"
#include "path.h"

Node *grid;
int gridwidth, gridheight;
double	(*distfn)(Node*, Node*);

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

int
isblocked(Node *n)
{
	return n->blocked;
}

void
clearpath(void)
{
	Node *n;

	if(grid == nil)
		return;
	for(n=grid; n<grid+gridwidth*gridheight; n++)
		memset(&n->PState, 0, sizeof n->PState);
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

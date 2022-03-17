#include <u.h>
#include <libc.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

Node *map;
int mapwidth, mapheight;

Vertex
n2p(Node *n)
{
	return (Vertex){(n - map) % mapwidth, (n - map) / mapwidth};
}

Node *
p2n(Vertex p)
{
	return map + p.y * mapwidth + p.x;
}

double
eucdist(Node *a, Node *b)
{
	int dx, dy;
	Vertex p, q;

	p = n2p(a);
	q = n2p(b);
	dx = p.x - q.x;
	dy = p.y - q.y;
	return sqrt(dx *dx + dy *dy);
}

double
octdist(Node *a, Node *b)
{
	int dx, dy;
	Vertex p, q;

	p = n2p(a);
	q = n2p(b);
	dx = abs(p.x - q.x);
	dy = abs(p.y - q.y);
	return 1 * (dx + dy) + MIN(dx, dy) * (SQRT2 - 2 * 1);
}

int
isblocked(Node *n)
{
	return n->blocked;
}

void
resetmap(void)
{
	Node *n;

	for(n=map; n<map+mapwidth*mapheight; n++)
		memset(&n->PNode, 0, sizeof n->PNode);
}

void
initmap(void)
{
	map = emalloc(mapwidth * mapheight * sizeof *map);
}

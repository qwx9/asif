#include <u.h>
#include <libc.h>
#include <draw.h>
#include "asif.h"
#include "graph.h"

typedef Vertex Point;

Vertex
V(int x, int y)
{
	return (Vertex){x, y};
}

Vectangle
V²(int ux, int uy, int vx, int vy)
{
	return (Vectangle){(Vertex){ux, uy}, (Vertex){vx, vy}};
}

Vectangle
toV²(Vertex u, Vertex v)
{
	return (Vectangle){(Vertex){u.x, u.y}, (Vertex){v.x, v.y}};
}

Vertex
ΔV(Vertex u, Vertex v)
{
	return (Vertex){v.x - u.x, v.y - u.y};
}

Vertex
∑V(Vertex u, Vertex v)
{
	return (Vertex){u.x + v.x, u.y + v.y};
}

int
V∩V²(Vertex u, Vectangle w)
{
	return u.x >= w.min.x && u.x < w.max.x
		&& u.y >= w.min.y && u.y < w.max.y;
}

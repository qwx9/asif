typedef struct Vertex Vertex;
typedef struct Node Node;
typedef struct PNode PNode;

struct Vertex{
	int x;
	int y;
};
struct PNode{
	int open;
	int closed;
	double g;
	double h;
	double Δg;
	Node *to;
	Node *from;
	Pairheap *pq;
};
enum{
	Nodesz = 8,
};
struct Node{
	int blocked;
	PNode;
};
extern Node *map;
extern int mapwidth, mapheight;
extern Node *selected;
extern Node *start, *goal;
extern int fourdir;

extern double	(*distfn)(Node*, Node*);
extern Node**	(*successorfn)(Node*);
extern Node*	(*pathfn)(Node*, Node*);

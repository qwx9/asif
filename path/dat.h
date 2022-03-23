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
struct Node{
	int blocked;
	PNode;		// generalize later
};
extern Node *map;
extern int mapwidth, mapheight;
extern Node *selected;
extern Node *start, *goal;

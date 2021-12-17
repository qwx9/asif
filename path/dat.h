typedef struct Vertex Vertex;
typedef struct Node Node;

struct Vertex{
	int x;
	int y;
};
struct Node{
	int blocked;
};
extern Node *map;
extern int mapwidth, mapheight;
extern Node *selected;

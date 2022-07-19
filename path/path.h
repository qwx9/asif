typedef struct State State;
typedef struct PState PState;
typedef struct Node Node;
typedef struct Vertex Vertex;
typedef struct Prof Prof;

struct Vertex{
	int x;
	int y;
};
struct PState{
	int open;
	int closed;
	Node *from;
	Node *to;
	void *aux;
};
struct State{
	int blocked;
	PState;
};
struct Node{
	Vertex;
	State;
};
int	isblocked(Node*);
void	toggleblocked(Node*);

enum{
	Deuclid,
	Dmanhattan,
	Doctile,
};
double	eucdist(Node*, Node*);
double	octdist(Node*, Node*);
double	manhdist(Node*, Node*);
extern double	(*distfn)(Node*, Node*);

Node*	p2n(Vertex);
Vertex	n2p(Node*);

enum{
	Move8,
	Move4,
};
extern int movemode;

extern Node *grid;
extern int gridwidth, gridheight;

void	clearpath(void);
void	cleargrid(void);
Node*	initgrid(int, int);

int	a∗findpath(Node*, Node*);
int	bfsfindpath(Node*, Node*);
int	dijkstrafindpath(Node*, Node*);

struct Prof{
	double dist;
	double cost;
	int steps;
	int touched;
	int opened;
	int updated;
	int closed;
};
extern Prof stats;
extern int doprof;

#pragma	varargck	type	"N"	Node*
#pragma	varargck	type	"V"	Vertex

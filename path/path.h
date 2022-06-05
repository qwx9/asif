typedef struct Vertex Vertex;
typedef struct State State;
typedef struct PState PState;
typedef struct Node Node;

struct Vertex{
	int x;
	int y;
};
struct PState{
	int open;
	int closed;
	Node *from;
	Node *to;
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

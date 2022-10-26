typedef struct State State;
typedef struct PState PState;
typedef struct Node Node;
typedef struct Prof Prof;

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

enum{
	θ∅ = 0,
	θ→ = 1<<0,
	θ↘ = 1<<1,
	θ↓ = 1<<2,
	θ↙ = 1<<3,
	θ← = 1<<4,
	θ↖ = 1<<5,
	θ↑ = 1<<6,
	θ↗ = 1<<7,
};

extern Node *grid;
extern int gridwidth, gridheight;

void	dprintpath(Node*, Node*);
void	clearpath(void);
void	cleargrid(void);
Node*	initgrid(int, int);
double	unitmovecost(Node*, Node*);
Node**	expand(Node*);

int	a∗findpath(Node*, Node*);
int	bfsfindpath(Node*, Node*);
int	dijkstrafindpath(Node*, Node*);

struct Prof{
	double dist;
	double cost;
	int steps;
	int opened;
	int expanded;
	int updated;
	int closed;
};
extern Prof stats;
extern int doprof;

#pragma	varargck	type	"N"	Node*
#pragma	varargck	type	"V"	Vertex

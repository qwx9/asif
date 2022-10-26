typedef struct Sim Sim;

extern Node *selected;
extern Node *start, *goal;

enum{
	Mmodegoal,
	Mmodestart,
	Mmodeblock,
	Mmodes,
};
extern int mousemode;

enum{
	Pbfs,
	Pdijkstra,
	Pa∗,
};

struct Sim{
	Prof;
	VArray *path;
	Vertex start;
	Vertex goal;
};
extern int nscen, curscen;
extern char *mapfile;
extern VArray *sims;

extern int nodesz;
extern int showgrid;

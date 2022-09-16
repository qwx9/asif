extern Node *selected;
extern Node *start, *goal;

enum{
	Pbfs,
	Pdijkstra,
	Pa∗,
};
extern int	(*pathfn)(Node*, Node*);

extern int nscen, scenid;

extern int nodesz;
extern int showgrid;

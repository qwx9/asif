enum{
	Nodesz = 2,
};
extern Node *selected;
extern Node *start, *goal;

enum{
	Pbfs,
	Pdijkstra,
	Pa∗,
};
extern int	(*pathfn)(Node*, Node*);

extern int nscen, scenid;

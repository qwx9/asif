enum{
	Nodesz = 4,
};
extern Node *selected;
extern Node *start, *goal;

enum{
	Pbfs,
	Pdijkstra,
	Pa∗,
};
extern int	(*pathfn)(Node*, Node*);

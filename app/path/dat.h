enum{
	Nodesz = 8,
};
extern Node *selected;
extern Node *start, *goal;

extern int	(*pathfn)(Node*, Node*);

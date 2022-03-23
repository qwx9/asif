void	init(int, char**);
void	evloop(void);
void	initfs(void);
void	resetmap(void);
void	clearmap(void);
void	initmap(void);
void	initdrw(void);
void	resetdrw(void);
double	eucdist(Node*, Node*);
double	octdist(Node*, Node*);
double	manhdist(Node*, Node*);
Vertex	n2p(Node*);
Node*	p2n(Vertex);
int	isblocked(Node*);

void	init(char*, char*, Vertex, int, int, int);
Node*	scrselect(Point);
void	updatedrw(void);
int	menter(char*, char*, int);
void	evloop(void);
void	showscen(int);
void	reloadscen(void);
void	runscens(void);
int	readscen(char*, char*, Vertex*, int*, int*, int*);
void	initfs(void);
int	Vfmt(Fmt*);
int	Nfmt(Fmt*);
int	initmap(char*, char*, Vertex, int, int, int);
void	initdrw(void);
void	resetdrw(void);
Vertex	n2s(Node*);
void	clearmap(void);
int	setparm(int, int, int);

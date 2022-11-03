void	initgraphics(int (*)(Rune), int (*)(Mouse, Point));
void	init(char*, Vertex, int, int, int);
Node*	scrselect(Point);
void	errmsg(char*, ...);
void	updatedrw(int, int);
int	menter(char*, char*, int);
void	evloop(void);
void	writeresults(void);
int	readresults(char*);
int	readscens(char*);
int	readmaphdr(char*, Vertex*);
int	readmap(char*);
void	initfs(void);
int	Vfmt(Fmt*);
int	Nfmt(Fmt*);
int	trypath(Node*, Node*);
int	showscen(int);
int	reloadscen(void);
void	runallscens(void);
int	initmap(char*, Vertex, int, int, int);
void	initdrw(void);
void	resetdrw(void);
Vertex	n2s(Node*);

#pragma	varargck	argpos	errmsg	1

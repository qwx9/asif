typedef struct String String;
struct String{
	char *s;
	int n;
};

typedef struct VArray VArray;
struct VArray{
	int n;
	int elsize;
	int vsize;
	int bufsize;
	void *p;
};
void	vfree(VArray*);
void	vnuke(VArray*);
void	vinsert(VArray*, char*);
VArray*	valloc(ulong, int);

typedef struct VShard VShard;
typedef struct Vector Vector;
struct VShard{
	uchar *p;
	uchar *head;
	int len;
	VShard *prev;
	VShard *next;
};
struct Vector{
	int len;
	int elsz;
	VShard vl;
	void *tmp;
};
void	vecfree(Vector*);
void*	vechpop(Vector*);
void*	vectpop(Vector*);
void	vecpush(Vector*, void*);
void*	vecget(Vector*, int);
Vector*	vec(int);

typedef struct Vecarr Vecarr;
struct Vecarr{
	void *buf;
	void *tail;
	int elsz;
	usize len;
	usize bufsz;
};

void	vecarrnuke(Vecarr*);
void*	vecarrget(Vecarr*, usize);
void*	vecarrpoptail(Vecarr*);
usize	vecarrindexof(Vecarr*, void*);
void*	vecarrcopy(Vecarr*, void*, usize*);
void*	vecarrp(Vecarr*, usize);
void	vecarrresize(Vecarr*, usize);
Vecarr	vecarr(usize, usize);

typedef struct QNode QNode;
struct QNode{
	QNode *left;
	QNode *right;
	QNode *up;
	QNode *down;
	void *aux;
};
QNode*	qtmerge(QNode*);
QNode*	qtsplit(QNode*);
QNode*	qtnew(void);

u32int	next32pow2(u32int);
int	lsb64(uvlong);
int	msb64(uvlong);

int	modpow(int, int, int);

VArray*	naivestrfind(String, String);
VArray*	morrispratt(String, String);
VArray*	knuthmorrispratt(String, String);
VArray*	rabinkarp(String, String, int, int);
VArray*	rabinkarp8(String, String);

enum{
	Hashsz = 101,
};
typedef struct HTab HTab;
typedef struct HPair HPair;
struct HPair{
	char *key;
	void *val;
	HPair *next;
};
struct HTab{
	HPair b[Hashsz];
};

int	htremovel(HTab*, u32int);
void*	htgetl(HTab*, u32int);
int	htputl(HTab*, u32int, void*);
int	htremove(HTab*, char*);
void*	htget(HTab*, char*);
int	htput(HTab*, char*, void*);
HTab*	htalloc(void);

typedef struct Pairheap Pairheap;
struct Pairheap{
	double n;
	void *aux;
	Pairheap *parent;
	Pairheap *left;
	Pairheap *right;
};
void	printqueue(Pairheap**);
void	nukequeue(Pairheap**);
Pairheap*	popqueue(Pairheap**);
void	decreasekey(Pairheap*, double, Pairheap**);
Pairheap*	pushqueue(double, void*, Pairheap**);

void	four1(double*, int, int);
void	realft(double*, int, int);

char*	estrdup(char*);
void*	erealloc(void*, ulong, ulong);
void*	emalloc(ulong);

typedef struct Zpool Zpool;
struct Zpool{
	usize elsize;
	void data[];
};
void	znuke(Zpool*);
void	zfree(void*);
void*	zalloc(Zpool*);
Zpool*	znew(usize);

#define MIN(a,b)	((a) <= (b) ? (a) : (b))
#define MAX(a,b)	((a) >= (b) ? (a) : (b))

#define SQRT2 1.4142135623730951

enum{
	Lognone,
	Logdebug,
	Logtrace,
	Logparanoid,
};
extern int debuglevel;

void	dprint(int, char *, ...);
#pragma	varargck	argpos	dprint	2

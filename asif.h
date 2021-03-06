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
void	vinsert(VArray*, char*);
VArray*	valloc(ulong, int);

u32int	next32pow2(u32int);
int	lsb64(uvlong);
int	msb64(uvlong);

int	modpow(int, int, int);

VArray*	naivestrfind(String, String);
VArray*	morrispratt(String, String);
VArray*	knuthmorrispratt(String, String);
VArray*	rabinkarp(String, String, int, int);
VArray*	rabinkarp8(String, String);

typedef struct Pairheap Pairheap;
struct Pairheap{
	double n;
	void *aux;
	Pairheap *parent;
	Pairheap *left;
	Pairheap *right;
};
void	nukequeue(Pairheap**);
Pairheap*	popqueue(Pairheap**);
void	decreasekey(Pairheap*, double, Pairheap**);
void	pushqueue(double, void*, Pairheap**);

void	four1(double*, int, int);
void	realft(double*, int, int);

void*	erealloc(void*, ulong);
void*	emalloc(ulong);

#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define MAX(a,b)	((a) > (b) ? (a) : (b))

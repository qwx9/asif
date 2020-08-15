typedef struct String String;
struct String{
	char *s;
	int n;
};

typedef struct VArray VArray;
struct VArray{
	int nelem;
	int elsize;
	int vsize;
	int bufsize;
	void *p;
};
void	vfree(VArray*);
VArray*	vinsert(VArray*, char*);
VArray*	valloc(ulong, int);

VArray*	naivestrfind(String, String);

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

void*	erealloc(void*, ulong, ulong);
void*	emalloc(ulong);

#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define MAX(a,b)	((a) > (b) ? (a) : (b))

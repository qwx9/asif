typedef struct VArray VArray;
typedef struct String String;

struct String{
	char *s;
	int n;
};

struct VArray{
	int nelem;
	int elsize;
	int vsize;
	int bufsize;
	void *p;
};

void*	erealloc(void*, ulong, ulong);
void*	emalloc(ulong);
void	vfree(VArray*);
VArray*	vinsert(VArray*, char*);
VArray*	valloc(ulong, int);

#define MIN(a,b)	((a) < (b) ? (a) : (b))
#define MAX(a,b)	((a) > (b) ? (a) : (b))


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

#include <u.h>
#include <libc.h>
#include "asif.h"

/* Fredman, M.L., Sedgewick, R., Sleator, D.D.  et al.  The pairing
 * heap: A new form of self-adjusting heap.  Algorithmica 1, 111–129
 * (1986).  */

static void
printright(Pairheap *p, int level)
{
	int i, j;
	Pairheap *q;

	if(p == nil)
		return;
	fprint(2, "(\n");
	for(i=0; i<level; i++)
		fprint(2, "\t");
	for(q=p, j=0; q!=nil; q=q->right, j++){
		fprint(2, "[%#p,%.5f]", q, q->n);
		printright(q->left, level+1);
	}
	fprint(2, "\n");
	for(i=0; i<level-1; i++)
		fprint(2, "\t");
	fprint(2, ")");
}
static void
printqueue(Pairheap *p)
{
	if(debuglevel == 0)
		return;
	if(p == nil)
		return;
	fprint(2, "pheap ");
	printright(p, 1);
	fprint(2, "\n");
}

static Pairheap *
mergequeue(Pairheap *a, Pairheap *b)
{
	if(b == nil)
		return a;
	else if(a->n < b->n){
		b->right = a->left;
		a->left = b;
		b->parent = a;
		return a;
	}else{
		a->right = b->left;
		b->left = a;
		a->parent = b;
		return b;
	}
}

static Pairheap *
mergepairs(Pairheap *a)
{
	Pairheap *b, *c;

	if(a == nil)
		return nil;
	a->parent = nil;
	b = a->right;
	if(b == nil)
		return a;
	a->right = nil;
	b->parent = nil;
	c = b->right;
	b->right = nil;
	return mergequeue(mergequeue(a, b), mergepairs(c));
}

void
nukequeue(Pairheap **queue)
{
	Pairheap *p;

	while((p = popqueue(queue)) != nil)
		free(p);
}

Pairheap *
popqueue(Pairheap **queue)
{
	Pairheap *p;

	p = *queue;
	if(p == nil)
		return nil;
	dprint(Logtrace, "pheap::popqueue %#p %.6f\n", p, p->n);
	*queue = mergepairs(p->left);
	return p;
}

void
decreasekey(Pairheap *p, double Δ, Pairheap **queue)
{
	Pairheap *q;

	dprint(Logtrace, "pheap::decreasekey %#p %.6f Δ %.6f\n", p, p->n, Δ);
	p->n -= Δ;
	if(p->parent != nil && p->n < p->parent->n){
		if(p->parent->left == p)
			p->parent->left = nil;
		else{
			for(q=p->parent->left; q->right!=p; q=q->right)
				;
			assert(q != nil);
			q->right = p->right;
		}
		p->parent = nil;
		*queue = mergequeue(p, *queue);
	}
}

Pairheap *
pushqueue(double n, void *aux, Pairheap **queue)
{
	Pairheap *p;

	p = emalloc(sizeof *p);
	p->n = n;
	p->aux = aux;
	*queue = mergequeue(p, *queue);
	dprint(Logtrace, "pheap::pushqueue %#p %.6f\n", p, p->n);
	return p;
}

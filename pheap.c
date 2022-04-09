#include <u.h>
#include <libc.h>
#include "asif.h"

/* Fredman, M.L., Sedgewick, R., Sleator, D.D.  et al.  The pairing
 * heap: A new form of self-adjusting heap.  Algorithmica 1, 111–129
 * (1986).  */

/* this implementation requires a bigger stack size if the heap can
 * grow big (8192 is already insufficient with 40-50 nodes);
 * otherwise, stack overflows hidden behind more cryptic memory pool
 * corruption errors will occur */

static void
checkheap(Pairheap *p, Pairheap *queue)
{
	Pairheap *q;

	if(p == nil || queue == nil)
		return;
	if(p == queue)
		fprint(2, "pheap::checkheap %#p\n", p);
	assert(p == queue || p->parent != nil && p->parent != p);
	for(q=p; q!=nil; q=q->right)
		checkheap(q->left, queue);
}

static void
printright(Pairheap *p, int level)
{
	int i;
	Pairheap *q;

	if(p == nil)
		return;
	fprint(2, "(\n");
	for(i=0; i<level; i++)
		fprint(2, "\t");
	for(q=p; q!=nil; q=q->right){
		fprint(2, "[%#p,%.5f]", q, q->n);
		printright(q->left, level+1);
	}
	fprint(2, "\n");
	for(i=0; i<level-1; i++)
		fprint(2, "\t");
	fprint(2, ")");
}
void
printqueue(Pairheap **queue)
{
	if(queue == nil)
		return;
	fprint(2, "pheap::printqueue %#p ", *queue);
	printright(*queue, 1);
	fprint(2, "\n");
}

static void
debugqueue(Pairheap **queue)
{
	if(debuglevel < Logparanoid || queue == nil)
		return;
	printqueue(queue);
	checkheap(*queue, *queue);
}

static Pairheap *
mergequeue(Pairheap *a, Pairheap *b)
{
	dprint(Logparanoid, "pheap::mergequeue %#p %.6f b %#p %.6f\n",
		a, a->n, b, b!=nil ? b->n : NaN());
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
mergepairs(Pairheap *a, Pairheap *q)
{
	Pairheap *b, *c;

	if(a == nil)
		return nil;
	if(a->parent != nil && a->parent->left == a)
		a->parent->left = nil;
	a->parent = nil;
	assert(a->parent == nil || a->parent->left != a);
	b = a->right;
	if(b == nil)
		return a;
	a->right = nil;
	if(b->parent != nil && b->parent->left == b)
		b->parent->left = nil;
	b->parent = nil;
	assert(b->parent == nil || b->parent->left != b);
	c = b->right;
	b->right = nil;
	return mergequeue(mergequeue(a, b), mergepairs(c, q));
}

void
nukequeue(Pairheap **queue)
{
	Pairheap *p;

	dprint(Logparanoid, "pheap::nukequeue %#p\n", queue);
	while((p = popqueue(queue)) != nil){
		debugqueue(&p);
		free(p);
	}
}

Pairheap *
popqueue(Pairheap **queue)
{
	Pairheap *p;

	p = *queue;
	if(p == nil)
		return nil;
	dprint(Logparanoid, "pheap::popqueue %#p %.6f\n", p, p->n);
	*queue = mergepairs(p->left, p);
	debugqueue(queue);
	return p;
}

void
decreasekey(Pairheap *p, double Δ, Pairheap **queue)
{
	Pairheap *q;

	dprint(Logparanoid, "pheap::decreasekey %#p %.6f Δ %.6f\n", p, p->n, Δ);
	p->n -= Δ;
	if(p->parent != nil && p->n < p->parent->n){
		assert(p->parent->left != nil);
		if(p->parent->left == p)
			p->parent->left = p->right;
		else{
			for(q=p->parent->left; q!=nil && q!=p; q=q->right)
				if(q->right == p)
					break;
			assert(q != nil);
			q->right = p->right;
		}
		p->parent = nil;
		*queue = mergequeue(p, *queue);
		debugqueue(queue);
	}
}

Pairheap *
pushqueue(double n, void *aux, Pairheap **queue)
{
	Pairheap *p;

	p = emalloc(sizeof *p);
	p->n = n;
	p->aux = aux;
	dprint(Logparanoid, "pheap::pushqueue %#p %.6f\n", p, p->n);
	*queue = mergequeue(p, *queue);
	debugqueue(queue);
	return p;
}

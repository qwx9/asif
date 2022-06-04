#include <u.h>
#include <libc.h>
#include "asif.h"

static Zpool *zpool;

QNode *
qtmerge(QNode *q)
{
	if(q == nil)
		return nil;
	qtmerge(q->left);
	qtmerge(q->right);
	qtmerge(q->up);
	qtmerge(q->down);
	zfree(q->left, zpool);
	zfree(q->right, zpool);
	zfree(q->up, zpool);
	zfree(q->down, zpool);
	q->left = q->right = q->up = q->down = nil;
	return q;
}

QNode *
qtsplit(QNode *q)
{
	if(q == nil)
		return nil;
	assert(q->left == nil && q->right == nil && q->up == nil && q->down == nil);
	q->left = zalloc(zpool);
	q->right = zalloc(zpool);
	q->up = zalloc(zpool);
	q->down = zalloc(zpool);
	return q;
}

QNode *
qtnew(void)
{
	QNode *q;

	q = emalloc(sizeof *q);
	if(zpool == nil)
		zpool = znew(sizeof *q);
	return q;
}

#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

Node *start, *goal;

static int
dijkstra(Node *a, Node *g)
{
	USED(a,g);
	return 0;
}

static int
findpath(void)
{
	if(start == nil || goal == nil)
		return -1;
	return dijkstra(start, goal);
}

int
mouseinput(Node *n, Mouse m)
{
	switch(m.buttons & 7){
	case 1: goal = n; findpath(); break;
	case 2: start = n; findpath(); break;
	case 4: n->blocked ^= 1; break;
	}
	return 0;
}

int
keyinput(Rune)
{
	return 0;
}

void
threadmain(int argc, char **argv)
{
	init(argc, argv);
	evloop();
}

#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include <pool.h>
#include "asif.h"
#include "path.h"
#include "dat.h"
#include "fns.h"

extern int	(*mousefn)(Mouse);
extern int	(*keyfn)(Rune);

mainstacksize = 512*1024;

Node *start, *goal;

static int
grmouse(Mouse m)
{
	static Node *old;
	Node *n;

	if(m.buttons == 0){
		old = nil;
		return 0;
	}
	if((n = scrselect(m.xy)) == nil || old == n)
		return 0;
	switch(m.buttons & 7){
	case 1:
		if(goal != n && !isblocked(n))
			start = n;
		break;
	case 2:
		if(start != n && !isblocked(n))
			goal = n;
		break;
	case 4:
		if(old == nil || isblocked(n) ^ isblocked(old))
			toggleblocked(n);
		break;
	}
	old = n;
	if(start != nil && goal != nil)
		if(pathfn(start, goal) < 0){
			dprint(Logdebug, "grid::findpath: findpath from [%#p,%P] to [%#p,%P]: %r\n",
				start, n2p(start), goal, n2p(goal));
		}
	return 1;
}

static int
grkey(Rune r)
{
	switch(r){
	case Kdel:
	case 'q': threadexitsall(nil);
	case 'r': cleargrid(); updatedrw(); break;
	}
	return 0;
}

static void
usage(void)
{
	fprint(2, "usage: %s [-D4] [-s width[,height]]\n", argv0);
	threadexits("usage");
}

void
threadmain(int argc, char **argv)
{
	int w, h, d, m;
	char *s;

	w = 64;
	h = 64;
	d = Move8;
	m = Doctile;
	ARGBEGIN{
	case 'D':
		if(++debuglevel >= Logparanoid)
			mainmem->flags |= POOL_NOREUSE | POOL_PARANOIA | POOL_LOGGING;
		break;
	case '4':
		d = Move4;
		m = Dmanhattan;
		break;
	case 's':
		w = strtol(EARGF(usage()), &s, 0);
		if(w <= 0)
			usage();
		if(*s != ','){
			h = w;
			break;
		}
		h = strtol(s+1, nil, 0);
		if(h <= 0)
			usage();
		break;
	default: usage();
	}ARGEND
	if(w <= 0 || w > 512
	|| h <= 0 || h > 512)
		sysfatal("invalid map size, must be in ]0,512]");
	keyfn = grkey;
	mousefn = grmouse;
	setparm(d, 0, m);
	init(w, h);
	evloop();
}

#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include <pool.h>
#include "asif.h"
#include "graph.h"
#include "path.h"
#include "dat.h"
#include "fns.h"

extern int	(*mousefn)(Mouse, Point);
extern void	dopan(Point);
extern Point pan;
extern int	(*keyfn)(Rune);

mainstacksize = 512*1024;

Node *start, *goal;

static setgoalmode;

static int
grmouse(Mouse m, Point Δ)
{
	static Node *old;
	Node *n;

	if(m.buttons == 0)
		return 0;
	else if((m.buttons & 7) == 2){
		dopan(Δ);
		updatedrw(1);
		return 1;
	}
	if((n = scrselect(m.xy)) == nil || old == n)
		return 0;
	switch(m.buttons & 7){
	case 1:
		if(old == nil || isblocked(n) ^ isblocked(old))
			toggleblocked(n);
		break;
	case 4:
		if(setgoalmode){
			if(start != n && !isblocked(n))
				goal = n;
		}else{
			if(goal != n && !isblocked(n))
				start = n;
		}
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
setscen(void)
{
	int n;
	char buf[128];

	snprint(buf, sizeof buf, "%d", scenid);
	if(menter("Scenario id?", buf, sizeof buf) < 0){
		fprint(2, "getscen: %r\n");
		return -1;
	}
	if((n = strtol(buf, nil, 10)) < 0 || n > nscen){
		fprint(2, "getscen: invalid id %s\n", buf);
		return -1;
	}
	scenid = n;
	showscen(n);
	return 0;
}

static int
grkey(Rune r)
{
	switch(r){
	case Kdel:
	case 'q':
		threadexitsall(nil);
	case 'r':
		if(doprof){
			reloadscen();
			showscen(scenid);
		}else
			cleargrid();
		updatedrw(0);
		break;
	case ' ':
	case '\n':
		setgoalmode ^= 1;
		break;
	case 'g':
		showgrid ^= 1;
		updatedrw(0);
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		if(!doprof)
			break;
		if(setscen() >= 0)
			updatedrw(0);
		break;
	case '+':
	case '=':
		if(nodesz < 1<<16){
			nodesz <<= 1;
			resetdrw();
		}
		break;
	case '-':
		if(nodesz > 1){
			nodesz >>= 1;
			resetdrw();
		}
		break;
	case 'z':
		pan = ZP;
		updatedrw(1);
		break;
	}
	return 0;
}

static void
usage(void)
{
	fprint(2, "usage: %s [-D4] [-a algo] [-d dist] [-s width[,height]] [-m scen]\n", argv0);
	threadexits("usage");
}

void
threadmain(int argc, char **argv)
{
	int w, h, a, d, m;
	char *s, *scenres, *scenmap;

	w = 64;
	h = 64;
	a = -1;
	d = -1;
	m = Move8;
	scenmap = nil;
	scenres = nil;
	ARGBEGIN{
	case 'D':
		if(++debuglevel >= Logparanoid)
			mainmem->flags |= POOL_NOREUSE | POOL_PARANOIA | POOL_LOGGING;
		break;
	case '4':
		m = Move4;
		d = Dmanhattan;
		break;
	case 'a':
		s = EARGF(usage());
		if(strcmp(s, "a∗") == 0)
			a = Pa∗;
		else if(strcmp(s, "dijkstra") == 0)
			a = Pdijkstra;
		else if(strcmp(s, "bfs") == 0)
			a = Pbfs;
		else{
			fprint(2, "unsupported algorithm\n");
			usage();
		}
		break;
	case 'd':
		s = EARGF(usage());
		if(strcmp(s, "octile") == 0)
			d = Doctile;
		else if(strcmp(s, "manhattan") == 0)
			d = Dmanhattan;
		else if(strcmp(s, "euclid") == 0)
			d = Deuclid;
		else{
			fprint(2, "unsupported distance function\n");
			usage();
		}
		break;
	case 'm':
		scenmap = EARGF(usage());
		break;
	case 'r':
		scenres = EARGF(usage());
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
	if(d < 0)
		d = m == Move8 ? Doctile : Dmanhattan;
	if(a < 0)
		a = Pa∗;
	keyfn = grkey;
	mousefn = grmouse;
	init(scenmap, scenres, (Vertex){w,h}, m, a, d);
	if(doprof && scenres == nil)
		runscens();
	evloop();
	threadexitsall(nil);
}

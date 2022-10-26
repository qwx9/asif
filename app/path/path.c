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

extern void	dopan(Point);
extern Point pan;

mainstacksize = 128*1024;

Node *start, *goal;
int mousemode;

static int
grmouse(Mouse m, Point Δ)
{
	static Node *old;
	Node *n;

	if(m.buttons == 0){
		old = nil;
		return 0;
	}else if((m.buttons & 7) == 2){
		dopan(Δ);
		updatedrw(1);
		return 1;
	}
	if((n = scrselect(m.xy)) == nil || old == n)
		return 0;
	switch(m.buttons & 7){
	case 1: break; /* just selecting the node */
	case 4:
		switch(mousemode){
		case Mmodegoal: if(n != start && !isblocked(n)) goal = n; break;
		case Mmodestart: if(n != goal && !isblocked(n)) start = n; break;
		case Mmodeblock:
			if(n != start && n != goal
			&& (old == nil || isblocked(n) ^ isblocked(old)))
				toggleblocked(n);
			break;
		}
	}
	old = n;
	trypath(start, goal);
	return 1;
}

static int
setscen(void)
{
	int n;
	char buf[128];

	snprint(buf, sizeof buf, "%d", curscen);
	if(menter("Scenario id?", buf, sizeof buf) < 0){
		fprint(2, "getscen: %r\n");
		return -1;
	}
	if((n = strtol(buf, nil, 10)) < 0 || n > nscen){
		fprint(2, "getscen: invalid id %s\n", buf);
		return -1;
	}
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
		reloadscen();
		updatedrw(0);
		break;
	case ' ':
	case '\n':
		mousemode = (mousemode + 1) % Mmodes;
		updatedrw(0);
		break;
	case 'g':
		showgrid ^= 1;
		updatedrw(0);
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
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
	fprint(2, "usage: %s [-D4p] [-a algo] [-d dist] [-s width[,height]] [-m map] [-S scen] [-r res]\n", argv0);
	threadexits("usage");
}

void
threadmain(int argc, char **argv)
{
	int w, h, a, d, m;
	char *s, *map, *scen, *res;

	w = -1;
	h = -1;
	a = -1;
	d = -1;
	m = -1;
	doprof = 0;
	map = nil;
	scen = nil;
	res = nil;
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
		map = EARGF(usage());
		break;
	case 'p':
		doprof = 1;
		break;
	case 'r':
		res = EARGF(usage());
		break;
	case 'S':
		scen = EARGF(usage());
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
	init(map, (Vertex){w,h}, m, a, d);
	if(scen != nil){
		if(map == nil)
			sysfatal("mat not run scenarios without a map");
		readscens(scen);
		if(doprof){
			runallscens();
			writeresults();
			threadexitsall(nil);
		}
		if(res != nil && readresults(res) < 0)
			dprint(Lognone, "path::readresults: %r\n");
	}
	initgraphics(grkey, grmouse);
	if(scen != nil){
		showscen(0);
		updatedrw(0);
	}
	evloop();
	threadexitsall(nil);
}

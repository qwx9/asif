#include <u.h>
#include <libc.h>
#include <bio.h>
#include "asif.h"
#include "path.h"
#include "dat.h"
#include "fns.h"

int nscen, scenid;

/* https://bitbucket.org/dharabor/pathfinding/src/gppc/gppc-2014/scenarios/ */
typedef struct Sim Sim;
struct Sim{
	Prof;
	Vertex start;
	Vertex goal;
};
static VArray *sims;
static char *scenmap;

void
showscen(int id)
{
	Sim *sp;

	assert(id >= 0 && id < nscen);
	sp = sims->p;
	sp += id;
	start = p2n(sp->start);
	goal = p2n(sp->goal);
	if(pathfn(start, goal) < 0)
		fprint(2, "runscens: findpath from %N to %N: %r\n",
			start, goal);
}

static void
readresults(char *path)
{
	int n;
	char *s, *arg[32];
	Biobuf *bf;
	Sim *sp, *se;

	if((bf = Bopen(path, OREAD)) == nil)
		sysfatal("readscen: %r");
	sp = sims->p;
	se = sp + sims->n;
	while(sp < se){
		if((s = Brdstr(bf, '\n', 1)) == nil)
			sysfatal("readresults: %r");
		if((n = getfields(s, arg, nelem(arg), 1, " \t")) != 8)
			sysfatal("invalid record length %d not 8", n);
		sp->cost = strtod(arg[7], nil);
		sp->steps = atoi(arg[2]);
		sp->touched = atoi(arg[3]);
		sp->opened = atoi(arg[4]);
		sp->updated = atoi(arg[5]);
		sp->closed = atoi(arg[6]);
		free(s);
		sp++;
	}
	Bterm(bf);
}

void
runscens(void)
{
	Sim *sp, *se;

	sp = sims->p;
	se = sp + sims->n;
	fprint(2, "id\tsteps\ttouched\texpanded\tupdated\topened\tcost\tdist\n");
	while(sp < se){
		start = p2n(sp->start);
		goal = p2n(sp->goal);
		if(pathfn(start, goal) < 0)
			fprint(2, "runscens: findpath from %N to %N: %r\n",
				start, goal);
		memcpy(sp, &stats, sizeof stats);
		fprint(2, "%zd\t%d\t%d\t%d\t%d\t%d\t%.3f\t%.3f\n",
			sp - (Sim*)sims->p,
			stats.steps, stats.touched, stats.opened,
			stats.updated, stats.closed, stats.cost, stats.dist);
		sp++;
	}
}

static int
readscenmaphdr(Biobuf *bf, Vertex *v)
{
	int done;
	char *s, *t;

	done = 0;
	while((s = Brdstr(bf, '\n', 1)) != nil){
		t = strtok(s, " ");
		if(strcmp(t, "type") == 0 || strcmp(t, "version") == 0){
			;
		}else if(strcmp(t, "height") == 0){
			if((t = strtok(nil, " ")) == nil)
				return -1;
			v->y = atoi(t);
		}else if(strcmp(t, "width") == 0){
			if((t = strtok(nil, " ")) == nil)
				return -1;
			v->x = atoi(t);
		}else if(strcmp(t, "map") == 0)
			done = 1;
		else{
			werrstr("unknown verb %s", t);
			return -1;
		}
		free(s);
		if(done)
			return 0;
	}
	return -1;
}

static int
readscenmap(char *path, Vertex *v)
{
	char c, *s, *t;
	Vertex u;
	Biobuf *bf;

	if((bf = Bopen(path, OREAD)) == nil)
		sysfatal("readscenmap: %r");
	if(readscenmaphdr(bf, v) < 0)
		return -1;
	if(gridwidth == 0)
		initgrid(v->x, v->y);
	cleargrid();
	for(u.y=0; u.y<gridheight; u.y++){
		if((s = Brdstr(bf, '\n', 1)) == nil)
			return -1;
		for(t=s, u.x=0; (c=*t)!=0; t++, u.x++){
			switch(c){
			case 'S':
			case 'W':
			case 'T':
			case '@':
			case 'O':
				toggleblocked(p2n(u));
				break;
			}
		}
		if(u.x != gridwidth){
			werrstr("line %d: invalid width %d not %d", u.y+1, u.x, gridwidth);
			return -1;
		}
		free(s);
	}
	Bterm(bf);
	return 0;
}

void
reloadscen(void)
{
	Vertex v;

	if(readscenmap(scenmap, &v) < 0)
		sysfatal("reloadscen: %r");
}

static int
readscenhdr(Biobuf *bf, Vertex *v)
{
	int done;
	char *s, *t;

	done = 0;
	while((s = Brdstr(bf, '\n', 1)) != nil){
		t = strtok(s, " ");
		if(strcmp(t, "type") == 0){
			;
		}else if(strcmp(t, "height") == 0){
			if((t = strtok(nil, " ")) == nil)
				return -1;
			v->y = atoi(t);
		}else if(strcmp(t, "width") == 0){
			if((t = strtok(nil, " ")) == nil)
				return -1;
			v->x = atoi(t);
		}else if(strcmp(t, "map") == 0)
			done = 1;
		else{
			werrstr("unknown verb %s", t);
			return -1;
		}
		free(s);
		if(done)
			return 0;
	}
	return -1;
}

int
readscen(char *path, char *respath, Vertex *v, int *m, int *a, int *d)
{
	int n;
	char *s, *arg[32];
	Biobuf *bf;
	Sim sim;

	if(path == nil)
		return 0;
	doprof = 1;
	/* only supported benchmarking configurations so far */
	if(*d != Doctile || *a != Pa∗ && *a != Pdijkstra || *m != Move8)
		sysfatal("unimplemented profiling for parameter set");
	if((s = strrchr(path, '.')) == nil){
		werrstr("invalid path name");
		return -1;
	}
	*s = 0;
	scenmap = estrdup(path);
	if(readscenmap(path, v) < 0)
		return -1;
	*s = '.';
	if((bf = Bopen(path, OREAD)) == nil)
		sysfatal("readscen: %r");
	sims = valloc(4100, sizeof(Sim));
	free(Brdstr(bf, '\n', 1));	/* "version 1\n" */
	while((s = Brdstr(bf, '\n', 1)) != nil){
		if((n = getfields(s, arg, nelem(arg), 1, " \t")) != 9){
			werrstr("invalid record length %d not 9", n);
			return -1;
		}
		sim.start.x = atoi(arg[4]);
		sim.start.y = atoi(arg[5]);
		sim.goal.x = atoi(arg[6]);
		sim.goal.y = atoi(arg[7]);
		sim.dist = strtod(arg[8], nil);
		vinsert(sims, (char*)&sim);
		nscen++;
		free(s);
	}
	Bterm(bf);
	if(nscen != 4100)
		sysfatal("scen file -- phase error");
	if(respath != nil){
		readresults(respath);
		showscen(0);
	}
	return 0;
}

void
initfs(void)
{
}

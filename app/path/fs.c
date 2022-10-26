#include <u.h>
#include <libc.h>
#include <bio.h>
#include "asif.h"
#include "graph.h"
#include "path.h"
#include "dat.h"
#include "fns.h"

char *mapfile;
int nscen, scenid;

/* https://bitbucket.org/dharabor/pathfinding/src/gppc/gppc-2014/scenarios/ */
enum{
	Nscen = 4100,
};
VArray *sims;

static VArray *
parsepath(char *s, Sim *sim)
{
	int n;
	u64int x;
	char *p, *t;
	VArray *v;
	Vertex px;

	n = 0;
	v = valloc(sim->steps, sizeof(Vertex));
	while((p = strchr(s, ',')) != nil){
		*p = 0;
		x = strtoull(s, &t, 10);
		if(t == s)
			sysfatal("parsepath: invalid node number");
		px = V(x % gridwidth, x / gridwidth);
		vinsert(v, (char*)&px);
		n++;
		s = p + 1;
	}
	if(n != sim->steps)
		sysfatal("parsepath -- phase error");
	return v;
}

int
readresults(char *path)
{
	int n;
	char *s, *arg[9];
	Biobuf *bf;
	Sim *sp, *se;

	assert(sims != nil);
	if((bf = Bopen(path, OREAD)) == nil)
		sysfatal("readresults: %r");
	sp = sims->p;
	se = sp + sims->n;
	while(sp < se){
		if((s = Brdstr(bf, '\n', 1)) == nil)
			sysfatal("readresults: %r");
		if((n = getfields(s, arg, nelem(arg), 1, " \t")) != nelem(arg)){
			werrstr("invalid record length %d not %d", n, nelem(arg));
			return -1;
		}
		sp->cost = strtod(arg[7], nil);
		sp->steps = atoi(arg[2]);
		sp->opened = atoi(arg[3]);
		sp->expanded = atoi(arg[4]);
		sp->updated = atoi(arg[5]);
		sp->closed = atoi(arg[6]);
		if(sp->steps <= 0)
			dprint(Lognone, "path::readresults: invalid entry line %zd\n",
				sp - (Sim *)sims->p);
		else
			sp->path = parsepath(arg[8], sp);
		free(s);
		sp++;
	}
	Bterm(bf);
	if(sp - (Sim *)sims->p != nscen){
		werrstr("results file -- phase error");
		return -1;
	}
	return 0;
}

void
writeresults(void)
{
	int i;
	Sim *sp, *se;
	Vertex *p, *e;

	print("id\tsteps\topened\texpanded\tupdated\texpanded\tcost\tdist\n");
	for(i=0, sp=sims->p, se=sp+sims->n; sp<se; sp++, i++){
		print("%d\t%d\t%d\t%d\t%d\t%d\t%.3f\t%.3f\t",
			i, sp->steps, sp->opened, sp->expanded,
			sp->updated, sp->closed, sp->cost, sp->dist);
		for(p=sp->path->p, e=p+sp->path->n; p<e; p++)
			print("%s%d", p == sp->path->p ? "" : ",",
				p->y * gridwidth + p->x);
		print("\n");
	}
}

int
readmaphdr(char *path, Vertex *dim)
{
	int r, done;
	char *s, *t;
	Biobuf *bf;

	if((bf = Bopen(path, OREAD)) == nil)
		sysfatal("readmaphdr: %r");
	done = 0;
	r = -1;
	while((s = Brdstr(bf, '\n', 1)) != nil){
		t = strtok(s, " ");
		if(strcmp(t, "type") == 0 || strcmp(t, "version") == 0){
			;
		}else if(strcmp(t, "height") == 0){
			if((t = strtok(nil, " ")) == nil){
				werrstr("invalid height field");
				goto end;
			}
			dim->y = atoi(t);
		}else if(strcmp(t, "width") == 0){
			if((t = strtok(nil, " ")) == nil){
				werrstr("invalid width field");
				goto end;
			}
			dim->x = atoi(t);
		}else if(strcmp(t, "map") == 0)
			done = 1;
		else{
			werrstr("unknown verb %s", t);
			goto end;
		}
		free(s);
		if(done){
			r = 0;
			break;
		}
	}
end:
	Bterm(bf);
	return r;
}

static int
skiphdr(Biobuf *bf)
{
	int done;
	char *s;

	while((s = Brdstr(bf, '\n', 1)) != nil){
		done = strcmp(s, "map") == 0;
		free(s);
		if(done)
			return 0;
	}
	werrstr("skipheader: skipped past eof");
	return -1;
}

int
readmap(char *path)
{
	char c, *s, *t;
	Vertex u;
	Biobuf *bf;

	if((bf = Bopen(path, OREAD)) == nil)
		sysfatal("readmap: %r");
	skiphdr(bf);
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

int
readscens(char *path)
{
	int n;
	char *s, *arg[9];
	Biobuf *bf;
	Sim sim;

	if((bf = Bopen(path, OREAD)) == nil)
		sysfatal("readscen: %r");
	memset(&sim, 0, sizeof sim);
	sims = valloc(Nscen, sizeof(Sim));
	free(Brdstr(bf, '\n', 1));	/* "version 1\n" */
	while((s = Brdstr(bf, '\n', 1)) != nil){
		if((n = getfields(s, arg, nelem(arg), 1, " \t")) < nelem(arg)){
			werrstr("invalid record length %d not %d", n, nelem(arg));
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
	if(nscen != Nscen)
		sysfatal("scenario file -- phase error");
	return 0;
}

void
initfs(void)
{
}

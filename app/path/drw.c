#include <u.h>
#include <libc.h>
#include <draw.h>
#include "asif.h"
#include "graph.h"
#include "path.h"
#include "dat.h"
#include "fns.h"

QLock drawlock;
Node *selected;
int showgrid;
int nodesz = 1;

typedef Vertex Point;
typedef Vectangle Rectangle;

enum{
	Cbg,
	Cgrid,
	Cfree,
	Cblocked,
	Copen,
	Cclosed,
	Cpath,
	Cstart,
	Cgoal,
	Cend,
};
static Image *col[Cend];
static Point viewΔ;
static Rectangle viewr, hudr;
static Image *view;

static Image *
eallocimage(Rectangle r, int repl, ulong col)
{
	Image *i;

	if((i = allocimage(display, r, screen->chan, repl, col)) == nil)
		sysfatal("allocimage: %r");
	return i;
}

Node *
scrselect(Point p)
{
	p = subpt(addpt(subpt(p, screen->r.min), viewΔ), Pt(1,1));
	if(!ptinrect(p, viewr)){
		selected = nil;
		return nil;
	}
	p = divpt(p, nodesz);
	p.x = MIN(p.x, gridwidth-1);
	p.y = MIN(p.y, gridheight-1);
	selected = grid + p.y * gridwidth + p.x;
	return selected;
}

static void
flushdrw(void)
{
	draw(screen, screen->r, view, nil, viewΔ);
	flushimage(display, 1);
}

static void
drawhud(void)
{
	char s[64], *sp;
	Node *n;

	draw(screen, hudr, col[Cbg], nil, ZP);
	sp = seprint(s, s+sizeof s, "grid size: %d,%d", viewr.max.x-1, viewr.max.y-1);
	if((n = selected) != nil){
		assert(n >= grid && n < grid + gridwidth * gridheight);
		sp = seprint(sp, s+sizeof s, " selected: %P%s",
			Pt((n-grid) % gridwidth, (n-grid) / gridwidth),
			isblocked(n) ? ": blocked" : "");
	}
	USED(sp);
	string(screen, addpt(screen->r.min, subpt(Pt(2, viewr.max.y+2), viewΔ)), col[Cfree], ZP, font, s);
}

static void
drawnodes(void)
{
	int sz;
	Node *n;
	Rectangle r;
	Image *c;

	sz = MAX(nodesz - showgrid, 1);
	for(n=grid; n<grid+gridwidth*gridheight; n++){
		if(isblocked(n))
			c = col[Cblocked];
		else if(n == start)
			c = col[Cstart];
		else if(n == goal)
			c = col[Cgoal];
		else if(n->to != nil)
			c = col[Cpath];
		else if(n->closed)
			c = col[Cclosed];
		else if(n->open)
			c = col[Copen];
		else
			continue;
		r.min = n2s(n);
		r.max = addpt(r.min, Pt(sz, sz));
		draw(view, r, c, nil, ZP);
	}
}

static void
drawfrom(void)
{
	Node *n;
	Point p0, p1;

	for(n=grid; n<grid+gridwidth*gridheight; n++){
		if(!n->open)
			continue;
		p1 = addpt(n2s(n), Pt(nodesz / 2, nodesz / 2));
		p0 = addpt(n2s(n->from), Pt(nodesz / 2, nodesz / 2));
		//line(view, p0, p1, Endarrow, 0, 0, col[Cgrid], ZP);
		line(view, p0, p1, 0, 0, 0, col[Cgrid], ZP);
	}
}

static void
drawgrid(void)
{
	Rectangle r;

	r = viewr;
	while(r.min.x < viewr.max.x){
		r.max.x = r.min.x;
		line(view, r.min, r.max, 0, 0, 0, col[Cgrid], ZP);
		r.min.x += nodesz;
	}
	r = viewr;
	while(r.min.y < viewr.max.y){
		r.max.y = r.min.y;
		line(view, r.min, r.max, 0, 0, 0, col[Cgrid], ZP);
		r.min.y += nodesz;
	}
}

static void
redraw(int clear)
{
	if(clear)
		draw(screen, screen->r, col[Cbg], nil, ZP);
	draw(view, view->r, col[Cfree], nil, ZP);
	if(showgrid && nodesz > 1)
		drawgrid();
	drawnodes();
	if(nodesz > 8)
		drawfrom();
}

void
updatedrw(int clear)
{
	qlock(&drawlock);
	redraw(clear);
	qunlock(&drawlock);
	drawhud();
	flushdrw();
}

void
resetdrw(void)
{
	viewr = Rpt(ZP, Pt(gridwidth*nodesz+1, gridheight*nodesz+1));
	viewΔ = divpt(addpt(subpt(ZP, subpt(screen->r.max, screen->r.min)), viewr.max), 2);
	hudr.min = addpt(screen->r.min, subpt(Pt(2, viewr.max.y+2), viewΔ));
	hudr.max = addpt(hudr.min, Pt(screen->r.max.x, font->height*3));
	freeimage(view);
	view = eallocimage(viewr, 0, DNofill);
	updatedrw(1);
}

void
initdrw(void)
{
	int z, zx, zy;

	if(initdraw(nil, nil, "path") < 0)
		sysfatal("initdraw: %r");
	col[Cbg] = display->black;
	col[Cgrid] = eallocimage(Rect(0,0,1,1), 1, 0x222222ff);
	col[Cblocked] = display->black;
	col[Cfree] = eallocimage(Rect(0,0,1,1), 1, 0x777777ff);
	col[Copen] = eallocimage(Rect(0,0,1,1), 1, 0x00ccccff);
	col[Cclosed] = eallocimage(Rect(0,0,1,1), 1, 0x0000ccff);
	col[Cpath] = eallocimage(Rect(0,0,1,1), 1, 0xcccc00ff);
	col[Cstart] = eallocimage(Rect(0,0,1,1), 1, 0x00cc00ff);
	col[Cgoal] = eallocimage(Rect(0,0,1,1), 1, 0xcc0000ff);
	zx = Dx(screen->r) / gridwidth;
	zy = Dy(screen->r) / gridheight;
	z = MIN(zx, zy);
	nodesz = z <= 1 ? 1 : z > 2 ? z & ~1 : z;
	resetdrw();
}

#include <u.h>
#include <libc.h>
#include <draw.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

QLock drawlock;
Node *selected;

typedef Vertex Point;

enum{
	Nodesz = 8,
};
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
	p = divpt(p, Nodesz);
	p.x = MIN(p.x, mapwidth-1);
	p.y = MIN(p.y, mapheight-1);
	selected = map + p.y * mapwidth + p.x;
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
	sp = seprint(s, s+sizeof s, "map size: %d,%d", viewr.max.x-1, viewr.max.y-1);
	if((n = selected) != nil){
		assert(n >= map && n < map + mapwidth * mapheight);
		sp = seprint(sp, s+sizeof s, " selected: %P%s",
			Pt((n-map) % mapwidth, (n-map) / mapwidth),
			n->blocked ? ": blocked" : "");
	}
	USED(sp);
	string(screen, addpt(screen->r.min, subpt(Pt(2, viewr.max.y+2), viewΔ)), col[Cfree], ZP, font, s);
}

static void
drawmap(void)
{
	Rectangle r;
	Node *n;

	draw(view, view->r, col[Cfree], nil, ZP);
	r = viewr;
	while(r.min.x < viewr.max.x){
		r.max.x = r.min.x;
		line(view, r.min, r.max, 0, 0, 0, col[Cgrid], ZP);
		r.min.x += Nodesz;
	}
	r = viewr;
	while(r.min.y < viewr.max.y){
		r.max.y = r.min.y;
		line(view, r.min, r.max, 0, 0, 0, col[Cgrid], ZP);
		r.min.y += Nodesz;
	}
	for(n=map; n<map+mapwidth*mapheight; n++)
		if(n->blocked){
			r.min.x = (n - map) % mapwidth * Nodesz + 1;
			r.min.y = (n - map) / mapwidth * Nodesz + 1;
			r.max = addpt(r.min, Pt(Nodesz-1, Nodesz-1));
			draw(view, r, col[Cblocked], nil, ZP);
		}
}

static void
redraw(void)
{
	drawmap();
}

void
updatedrw(void)
{
	qlock(&drawlock);
	redraw();
	qunlock(&drawlock);
	drawhud();
	flushdrw();
}

void
resetdrw(void)
{
	viewr = Rpt(ZP, Pt(mapwidth*Nodesz+1, mapheight*Nodesz+1));
	viewΔ = divpt(addpt(subpt(ZP, subpt(screen->r.max, screen->r.min)), viewr.max), 2);
	hudr.min = addpt(screen->r.min, subpt(Pt(2, viewr.max.y+2), viewΔ));
	hudr.max = addpt(hudr.min, Pt(viewr.max.x-2, font->height*3));
	freeimage(view);
	view = eallocimage(viewr, 0, DNofill);
	draw(screen, screen->r, col[Cbg], nil, ZP);
	updatedrw();
}

void
initdrw(void)
{
	if(initdraw(nil, nil, "path") < 0)
		sysfatal("initdraw: %r");
	col[Cbg] = display->black;
	col[Cgrid] = eallocimage(Rect(0,0,1,1), 1, 0x222222ff);
	col[Cblocked] = display->black;
	col[Cfree] = eallocimage(Rect(0,0,1,1), 1, 0x777777ff);
	col[Copen] = eallocimage(Rect(0,0,1,1), 1, 0x00ccccff);
	col[Cclosed] = eallocimage(Rect(0,0,1,1), 1, 0x0000ccff);
	col[Cpath] = eallocimage(Rect(0,0,1,1), 1, 0x777777ff);
	col[Cstart] = eallocimage(Rect(0,0,1,1), 1, 0x00cc00ff);
	col[Cgoal] = eallocimage(Rect(0,0,1,1), 1, 0xcc0000ff);
	resetdrw();
}

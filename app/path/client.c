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

typedef Vertex Point;

static int	(*mousefn)(Mouse, Point);
static int	(*keyfn)(Rune);
static Keyboardctl *kc;
static Mousectl *mc;

int
menter(char *label, char *buf, int bufsz)
{
	if(enter(label, buf, bufsz, mc, kc, nil) < 0)
		return -1;
	return 0;
}

void
evloop(void)
{
	Rune r;
	Mouse mold;

	enum{
		Aresize,
		Amouse,
		Akbd,
		Aend,
	};
	Alt a[] = {
		[Aresize] {mc->resizec, nil, CHANRCV},
		[Amouse] {mc->c, &mc->Mouse, CHANRCV},
		[Akbd] {kc->c, &r, CHANRCV},
		[Aend] {nil, nil, CHANEND},
	};
	for(;;){
		switch(alt(a)){
		case Aresize:
			if(getwindow(display, Refnone) < 0)
				sysfatal("resize failed: %r");
			resetdrw();
			mold = mc->Mouse;
			break;
		case Amouse:
			if(mousefn(mc->Mouse, subpt(mc->Mouse.xy, mold.xy)))
				updatedrw(0);
			mold = mc->Mouse;
			break;
		case Akbd:
			keyfn(r);
			break;
		}
	}
}

void
initgraphics(int (*kfn)(Rune), int (*mfn)(Mouse, Point))
{
	keyfn = kfn;
	mousefn = mfn;
	initdrw();
	if((kc = initkeyboard(nil)) == nil)
		sysfatal("initkeyboard: %r");
	if((mc = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
}

void
init(char *map, Vertex dim, int m, int a, int d)
{
	fmtinstall('P', Pfmt);
	fmtinstall('R', Rfmt);
	fmtinstall('V', Vfmt);
	fmtinstall('N', Nfmt);
	initfs();
	initmap(map, dim, m, a, d);
}

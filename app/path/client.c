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

int	(*mousefn)(Mouse);
int	(*keyfn)(Rune);

static Keyboardctl *kc;
static Mousectl *mc;

void
evloop(void)
{
	Rune r;

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
			break;
		case Amouse:
			if(mousefn(mc->Mouse))
				updatedrw();
			break;
		case Akbd:
			keyfn(r);
			break;
		}
	}
}

void
init(char *scen, Vertex v, int m, int a, int d)
{
	fmtinstall('P', Pfmt);
	fmtinstall('R', Rfmt);
	initfs();
	if(initmap(scen, v, m, a, d) < 0)
		sysfatal("init: %r");
	initdrw();
	if((kc = initkeyboard(nil)) == nil)
		sysfatal("initkeyboard: %r");
	if((mc = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
}

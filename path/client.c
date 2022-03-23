#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include <pool.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

extern QLock drawlock;
int	mouseinput(Node*, Mouse);
int	keyinput(Rune);
Node*	scrselect(Point);
void	updatedrw(void);

static Keyboardctl *kc;
static Mousectl *mc;

void
evloop(void)
{
	Rune r;
	Mouse m;
	Node *n, *p;

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
	p = nil;
	for(;;){
		switch(alt(a)){
		case Aresize:
			if(getwindow(display, Refnone) < 0)
				sysfatal("resize failed: %r");
			resetdrw();
			break;
		case Amouse:
			if(mc->buttons == 0)
				break;
			if((n = scrselect(m.xy)) != nil && p != n)
				mouseinput(n, mc->Mouse);
			p = n;
			updatedrw();
			break;
		case Akbd:
			switch(r){
			case Kdel: threadexitsall(nil);
			case 'r': clearmap(); updatedrw(); break;
			}
			keyinput(r);
			break;
		}
		m = mc->Mouse;
	}
}

static void
usage(void)
{
	fprint(2, "usage: %s [-s width[,height]]\n", argv0);
	threadexits("usage");
}

void
init(int argc, char **argv)
{
	char *s;

	mapwidth = 64;
	mapheight = 64;
	ARGBEGIN{
	case 's':
		mapwidth = strtol(EARGF(usage()), &s, 0);
		if(mapwidth <= 0)
			usage();
		if(*s != ','){
			mapheight = mapwidth;
			break;
		}
		mapheight = strtol(s+1, nil, 0);
		if(mapheight <= 0)
			usage();
		break;
	default: usage();
	}ARGEND
	//mainmem->flags |= POOL_PARANOIA | POOL_NOREUSE;
	fmtinstall('P', Pfmt);
	fmtinstall('R', Rfmt);
	initfs();
	initmap();
	initdrw();
	if((kc = initkeyboard(nil)) == nil)
		sysfatal("initkeyboard: %r");
	if((mc = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
}

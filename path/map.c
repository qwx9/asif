#include <u.h>
#include <libc.h>
#include "../asif.h"
#include "dat.h"
#include "fns.h"

Node *map;
int mapwidth, mapheight;

void
initmap(void)
{
	map = emalloc(mapwidth * mapheight * sizeof *map);
}

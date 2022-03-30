#include <u.h>
#include <libc.h>
#include "asif.h"

int debuglevel;

void
dprint(char *fmt, ...)
{
	char s[256];
	va_list arg;

	if(debuglevel == Lognone)
		return;
	va_start(arg, fmt);
	vseprint(s, s+sizeof s, fmt, arg);
	va_end(arg);
	fprint(2, "%s", s);
}

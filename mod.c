#include <u.h>
#include <libc.h>
#include "asif.h"

/* modular exponentiation by repeated binary square and multiply,
 * (addition chaining), right-to-left scan.
 * assumptions: mod > 0, base >= 0, exp >= 0
 * if base ≥ 0x10000, base² in the first iteration will overflow.
 * if mod > 0x10000, base can be ≥ 0x10000 in subsequent iterations.
 */
int
modpow(int base, int exp, int mod)
{
	int r;

	if(base == 0)
		return 0;
	assert(base < 0x10000);
	assert(mod <= 0x10000);
	r = 1;
	while(exp > 0){
		if(exp & 1)
			r = r * base % mod;
		exp >>= 1;
		base = base * base % mod;
	}
	return r;
}

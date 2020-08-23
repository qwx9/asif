#include <u.h>
#include <libc.h>
#include "asif.h"

/* rabin-karp for single pattern matching
 * d: base
 * q: modulo factor
 * D: multiplication factor in the recurrence formula:
 *	h = (d * (h - D * S[i]) + S[i+M]) % q
 *	D = d ** (M-1) % q
 */

/* http://monge.univ-mlv.fr/~lecroq/string/node5.html:
 * fast specialized version for 8-byte values, using the full range of a 32bit integer,
 * avoiding divisions, simplifying the precalculation of D and reducing most operations
 * to bit shifts.
 * assumed d = 2, q = 2³³
 * d*q < 2³³ not necessary since no intermediate values are stored
 *
 * some small modifications compared to the site to hopefully correct some issues:
 * - avoid out of bounds on last iteration
 * - calculate D in one step and avoid shifting beyond 32 bits
 */
VArray *
rabinkarp8(String W, String S)
{
	int i, n, ds, hw, hs;
	VArray *v;

	n = S.n - W.n + 1;
	if(n <= 0)
		return nil;
	v = valloc(n, sizeof(int));
	ds = W.n - 1 & 0x1f;
	hw = 0;
	hs = 0;
	for(i=0; i<W.n; i++){
		hw = W.s[i] + (hw << 1);
		hs = S.s[i] + (hs << 1);
	}
	for(i=0, n--;; i++){
		if(hw == hs && memcmp(W.s, S.s+i, W.n) == 0)
			vinsert(v, (void*)&i);
		if(i >= n)
			break;
		hs = S.s[i+W.n] + (hs - (S.s[i] << ds) << 1);
	}
	return v;
}

/* general algorithm */
VArray *
rabinkarp(String W, String S, int d, int q)
{
	int i, n, D, hw, hs;
	VArray *v;

	n = S.n - W.n + 1;
	if(n <= 0)
		return nil;
	v = valloc(n, sizeof(int));
	D = modpow(d, W.n - 1, q);
	hw = 0;
	hs = 0;
	for(i=0; i<W.n; i++){
		hw = (W.s[i] + d * hw) % q;
		hs = (S.s[i] + d * hs) % q;
	}
	for(i=0, n--;; i++){
		if(hw == hs && memcmp(W.s, S.s+i, W.n) == 0)
			vinsert(v, (void*)&i);
		if(i >= n)
			break;
		hs = (S.s[i+W.n] + d * (hs - D * S.s[i])) % q;
	}
	return v;
}

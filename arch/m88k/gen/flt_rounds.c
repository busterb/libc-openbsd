/*	$OpenBSD: flt_rounds.c,v 1.4 2012/06/25 17:01:11 deraadt Exp $	*/

/*
 * Written by J.T. Conklin, Apr 10, 1995
 * Public domain.
 */

#include <sys/types.h>
#include <float.h>

/*
 * Ported to 88k (Nivas Madhur)
 */

static const int map[] = {
	1,	/* round to nearest */
	0,	/* round to zero */
	3,	/* round to negative infinity */
	2	/* round to positive infinity */
};

int
__flt_rounds()
{
	int x;

	__asm__("fldcr %0, %%fcr63" : "=r" (x));
	return map[(x >> 14) & 0x03];
}

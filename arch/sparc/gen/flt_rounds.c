/*	$OpenBSD: flt_rounds.c,v 1.5 2007/10/27 20:02:59 miod Exp $ */
/*
 * Written by J.T. Conklin, Apr 10, 1995
 * Public domain.
 */

#include <sys/types.h>
#include <float.h>

static const int map[] = {
	1,	/* round to nearest */
	0,	/* round to zero */
	2,	/* round to positive infinity */
	3	/* round to negative infinity */
};

int
__flt_rounds()
{
	int x;

	__asm__("st %%fsr,%0" : "=m" (*&x));
	return map[(x >> 30) & 0x03];
}

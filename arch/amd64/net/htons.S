/*	$OpenBSD: htons.S,v 1.1 2004/01/28 01:44:45 mickey Exp $	*/

/* Written by Artur Grabowski. Public Domain */

#include <machine/asm.h>

ENTRY(_C_LABEL(htons))
	movl    %edi,%eax
	xchgb   %ah,%al
	ret


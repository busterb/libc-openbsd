#if defined(LIBC_SCCS) && !defined(lint)
/*static char *sccsid = "from: @(#)strlen.c	5.5 (Berkeley) 1/26/91";*/
static char *rcsid = "$Id$";
#endif /* LIBC_SCCS and not lint */

#include <string.h>

char *
strrchr(str, c)
	const char *str;
{
	rindex(str, c);
}

#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char	elsieid[] = "@(#)scheck.c	8.13";
#else
static char rcsid[] = "$OpenBSD: scheck.c,v 1.3 1996/08/19 08:34:51 tholo Exp $";
#endif
#endif /* LIBC_SCCS and not lint */

/*LINTLIBRARY*/

#include "private.h"

extern char *	imalloc P((int n));
extern void	ifree P((char * p));

char *
scheck(string, format)
const char * const	string;
char * const		format;
{
	register char *		fbuf;
	register const char *	fp;
	register char *		tp;
	register int		c;
	register char *		result;
	char			dummy;
	static char		nada;

	result = &nada;
	if (string == NULL || format == NULL)
		return result;
	fbuf = imalloc((int) (2 * strlen(format) + 4));
	if (fbuf == NULL)
		return result;
	fp = format;
	tp = fbuf;
	while ((*tp++ = c = *fp++) != '\0') {
		if (c != '%')
			continue;
		if (*fp == '%') {
			*tp++ = *fp++;
			continue;
		}
		*tp++ = '*';
		if (*fp == '*')
			++fp;
		while (is_digit(*fp))
			*tp++ = *fp++;
		if (*fp == 'l' || *fp == 'h')
			*tp++ = *fp++;
		else if (*fp == '[')
			do *tp++ = *fp++;
				while (*fp != '\0' && *fp != ']');
		if ((*tp++ = *fp++) == '\0')
			break;
	}
	*(tp - 1) = '%';
	*tp++ = 'c';
	*tp = '\0';
	if (sscanf(string, fbuf, &dummy) != 1)
		result = (char *) format;
	ifree(fbuf);
	return result;
}

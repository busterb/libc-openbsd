/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char rcsid[] = "$OpenBSD: usleep.c,v 1.3 1996/08/19 08:27:19 tholo Exp $";
#endif /* LIBC_SCCS and not lint */

#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

#define	TICK	10000		/* system clock resolution in microseconds */

static void sleephandler __P((int));
static volatile int ringring;

void
usleep(useconds)
	unsigned int useconds;
{
	struct itimerval itv, oitv;
	struct sigaction act, oact;
	sigset_t set, oset;

	if (!useconds)
		return;

	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_BLOCK, &set, &oset);

	act.sa_handler = sleephandler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM, &act, &oact);

	timerclear(&itv.it_interval);
	itv.it_value.tv_sec = useconds / 1000000;
	itv.it_value.tv_usec = useconds % 1000000;
	setitimer(ITIMER_REAL, &itv, &oitv);

	if (timerisset(&oitv.it_value)) {
		if (timercmp(&oitv.it_value, &itv.it_value, >)) {
			timersub(&oitv.it_value, &itv.it_value, &oitv.it_value);
		} else {
			itv.it_value = oitv.it_value;
			/*
			 * This is a hack, but we must have time to return
			 * from the setitimer after the alarm or else it'll
			 * be restarted.  And, anyway, sleep never did
			 * anything more than this before.
			 */
			oitv.it_value.tv_sec = 0;
			oitv.it_value.tv_usec = 2 * TICK;

			setitimer(ITIMER_REAL, &itv, NULL);
		}
	}

	set = oset;
	sigdelset(&set, SIGALRM);
	ringring = 0;
 	(void) sigsuspend(&set);

	if (!ringring) {
		struct itimerval nulltv;
		/*
		 * Interrupted by other signal; allow for pending 
		 * SIGALRM to be processed before resetting handler,
		 * after first turning off the timer.
		 */
		timerclear(&nulltv.it_interval);
		timerclear(&nulltv.it_value);
		(void) setitimer(ITIMER_REAL, &nulltv, NULL);
	}
	sigprocmask(SIG_SETMASK, &oset, NULL);
	sigaction(SIGALRM, &oact, NULL);
	(void) setitimer(ITIMER_REAL, &oitv, NULL);
}

/* ARGSUSED */
static void
sleephandler(sig)
	int sig;
{
	ringring = 1;
}

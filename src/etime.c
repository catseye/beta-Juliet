/*
 * Copyright (c)2004-2010 Cat's Eye Technologies.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * 
 *   Neither the name of Cat's Eye Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
/*
 * etime.c
 * Time/duration abstraction for 2Iota.
 * $Id: etime.c 519 2010-04-28 19:30:24Z cpressey $
 */

#include <time.h>
#ifdef _POSIX_C_SOURCE
  #include <sys/time.h>
#else
  #include <unistd.h>
#endif
#include <stdio.h>

#include "etime.h"

struct etime etime_zero = { 0, 0 };

void
etime_set(struct etime *et, long days, long hrs, long mins, long secs,
	  long msecs, long usecs)
{
	et->sec = secs + mins * 60 + hrs * 3600 + days * (24*3600);
	et->nsec = usecs * 1000 + msecs * 1000000;
}

void
etime_add(struct etime *et, long days, long hrs, long mins, long secs,
	  long msecs, long usecs)
{
	et->sec += secs + mins * 60 + hrs * 3600 + days * (24*3600);
	et->nsec += usecs * 1000 + msecs * 1000000;
}

void
etime_now(struct etime *et)
{
#if _POSIX_C_SOURCE
	struct timeval t;

	gettimeofday(&t, NULL);
	et->sec = t.tv_sec;
	et->nsec = t.tv_usec * 1000;
#else
	time_t t;

	t = time(NULL);
	et->sec = t;
	et->nsec = 0;
#endif
}

/*
 * Returns:
 *	-1 if a is a time before b;
 *	1 if a is a time after b;
 *	0 if the times a and b are exactly equal.
 */
int
etime_compare(struct etime *a, struct etime *b)
{
	if (a->sec == b->sec) {
		if (a->nsec == b->nsec)
			return(0);
		return(a->nsec < b->nsec ? -1 : 1);
	} else {
		return(a->sec < b->sec ? -1 : 1);
	}
}

void
etime_delta(struct etime *dest, struct etime *delta, int sign)
{
	dest->sec += (sign * delta->sec);
	dest->nsec += (sign * delta->nsec);	
}

int
etime_sleep(struct etime *et)
{
#ifdef _POSIX_C_SOURCE
	struct timespec tv;

	tv.tv_sec = et->sec;
	tv.tv_nsec = et->nsec;

	return(nanosleep(&tv, NULL));
#else
	return(sleep(et->sec));
#endif
}

void
etime_dump(struct etime *et)
{
	long days, hrs, mins, secs, msecs, usecs;
	struct etime t;

	t.sec = et->sec;
	t.nsec = et->nsec;

	days = t.sec / (24*3600);	t.sec %= (24*3600);
	hrs  = t.sec / 3600;		t.sec %= 3600;
	mins = t.sec / 60;		t.sec %= 60;
	secs = t.sec;

	msecs = t.nsec / 1000000;	t.nsec %= 1000000;
	usecs = t.nsec / 1000;
	
	fprintf(stderr, "[%02ld/%02ld:%02ld:%02ld.%04ld.%04ld]",
	    days, hrs, mins, secs, msecs, usecs);
}

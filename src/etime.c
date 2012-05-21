/*
 * Copyright (c)2004 Cat's Eye Technologies.  All rights reserved.
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
 * $Id: etime.c 54 2004-04-23 22:51:09Z catseye $
 */

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#include "etime.h"

struct etime etime_zero = {{ 0, 0 }};

void
etime_set(struct etime *et, long days, long hrs, long mins, long secs,
	  long msecs, long usecs)
{
	et->tv.tv_sec = secs + mins * 60 + hrs * 3600 + days * (24*3600);
	et->tv.tv_nsec = usecs * 1000 + msecs * 1000000;
}

void
etime_add(struct etime *et, long days, long hrs, long mins, long secs,
	  long msecs, long usecs)
{
	et->tv.tv_sec += secs + mins * 60 + hrs * 3600 + days * (24*3600);
	et->tv.tv_nsec += usecs * 1000 + msecs * 1000000;
}

void
etime_now(struct etime *et)
{
	struct timeval t;

	gettimeofday(&t, NULL);
	et->tv.tv_sec = t.tv_sec;
	et->tv.tv_nsec = t.tv_usec * 1000;
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
	if (a->tv.tv_sec == b->tv.tv_sec) {
		if (a->tv.tv_nsec == b->tv.tv_nsec)
			return(0);
		return(a->tv.tv_nsec < b->tv.tv_nsec ? -1 : 1);
	} else {
		return(a->tv.tv_sec < b->tv.tv_sec ? -1 : 1);
	}
}

void
etime_delta(struct etime *dest, struct etime *delta, int sign)
{
	dest->tv.tv_sec += (sign * delta->tv.tv_sec);
	dest->tv.tv_nsec += (sign * delta->tv.tv_nsec);	
}

int
etime_sleep(struct etime *et)
{
	return(nanosleep(&et->tv, NULL));
}

void
etime_dump(struct etime *et)
{
	long days, hrs, mins, secs, msecs, usecs;
	struct timespec t;

	t = et->tv;
	
	days = t.tv_sec / (24*3600);	t.tv_sec %= (24*3600);
	hrs  = t.tv_sec / 3600;		t.tv_sec %= 3600;
	mins = t.tv_sec / 60;		t.tv_sec %= 60;
	secs = t.tv_sec;

	msecs = t.tv_nsec / 1000000;	t.tv_nsec %= 1000000;
	usecs = t.tv_nsec / 1000;
	
	fprintf(stderr, "[%02ld/%02ld:%02ld:%02ld.%04ld.%04ld]",
	    days, hrs, mins, secs, msecs, usecs);
}

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
 * debug.c
 * Debugging (dumping, tracing...) routines for 2Iota.
 * $Id: debug.c 518 2010-04-28 17:48:38Z cpressey $
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "symbol.h"
#include "symstr.h"
#include "event.h"
#include "equeue.h"

#include "debug.h"
#include "cli.h"		/* XXX bleah */
#include "2iota.h"

int	trace_flags = 0;

void
debug_trace(int flag, const char *fmt, ...)
{
	va_list args;
	int i;

	if ((flag & trace_flags) != flag)
		return;

	va_start(args, fmt);
	switch (flag) {
	case TRACE_EVENTS:
		fprintf(stderr, "[ EVENT ] ");
		break;
	case TRACE_QUEUE:
		fprintf(stderr, "[ QUEUE ] ");
		break;
	case TRACE_HISTORY:
		fprintf(stderr, "[HISTORY] ");
		break;
	}

	for (i = 0; fmt[i] != '\0'; i++) {
		if (fmt[i] == '%') {
			i++;
			switch (fmt[i]) {
			case 'S':
				symstr_dump(va_arg(args, struct symstr *));
				break;
			case 'T':
				etime_dump(va_arg(args, struct etime *));
				break;
			case 'E':
				event_dump(va_arg(args, struct event *));
				break;
			case 'B':
				fprintf(stderr, va_arg(args, int) ? "TRUE" : "FALSE");
				break;
			case 's':
				fprintf(stderr, "%s", va_arg(args, char *));
				break;
			case 'd':
				fprintf(stderr, "%d", va_arg(args, int));
				break;
			}
		} else {
			fprintf(stderr, "%c", fmt[i]);
		}
	}
	fprintf(stderr, "\n");
}

void
debug_dump(const char *what)
{
	if (!strcmp(what, "symbols"))
		symbol_table_dump(0, gstab);
	else if (!strcmp(what, "alphabets"))
		symbol_table_dump(0, astab);
	else if (!strcmp(what, "events"))
		event_table_dump(event_table);
	else if (!strcmp(what, "queue"))
		equeue_dump(event_queue);
	else if (!strcmp(what, "history"))
		ehist_dump(event_hist);
	else if (strlen(what) == 0 || !strcmp(what, "all"))
		debug_dump_all();
	else {
		fprintf(stderr, "Unknown program element `%s'\n", what);
		help();
	}
}

void
debug_dump_all(void)
{
	fprintf(stderr, "*** General Symbols ***\n");
	debug_dump("symbols");
	fprintf(stderr, "*** Alphabets ***\n");
	debug_dump("alphabets");
	fprintf(stderr, "*** Events ***\n");
	debug_dump("events");
	fprintf(stderr, "*** Event Queue ***\n");
	debug_dump("queue");
	fprintf(stderr, "*** Event History ***\n");
	debug_dump("history");
}

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
 * main.c
 * 2iota - Reference interpreter for the 2Iota programming language.
 * $Id: main.c 545 2010-04-30 14:27:14Z cpressey $
 */

#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "scan.h"
#include "parse.h"
#include "symbol.h"
#include "symstr.h"
#include "event.h"
#include "equeue.h"
#include "ehist.h"

#include "engine.h"
#include "cli.h"
#include "debug.h"
#include "2iota.h"

/* GLOBALS */

struct symbol_table	*gstab;		/* general symbol table */
struct symbol_table	*astab;		/* symbol table for alphabets */
struct event_table	*event_table;	/* global table of events */
struct equeue		*event_queue;	/* global event queue */
struct ehist		*event_hist;	/* global event history log */

int			interactive = 0;

/* FUNCTIONS */

static void
usage(void)
{
	fprintf(stderr, "usage: 2iota [-dit] {-c event} srcfile ...\n");
	fprintf(stderr, "       -d: Dump loaded events and alphabets\n");
	fprintf(stderr, "       -i: Start interactive 2iota command line\n");
	fprintf(stderr, "       -t: Trace events, queue, and history\n");
	fprintf(stderr, " -c event: Cause named event initially\n");
	exit(1);
}

/* MAIN */

int
main(int argc, char **argv)
{
	struct scan_st *sc;
	struct symstr *ss;
	int do_dump = 0;
	int argn;

#ifdef DETECT_LEAKS
	GC_find_leak = 1;
#endif

	/* Initialize / allocate globals. */
	
	gstab = symbol_table_new();
	astab = symbol_table_new();
	event_table = event_table_new();
	event_queue = equeue_new();
	event_hist = ehist_new();

	/* Parse arguments. */

	for (argn = 1; argn < argc; argn++) {
                if (argv[argn][0] == '-') {
			switch(argv[argn][1]) {
			case 'c':	/* cause initial event */
				argn++;
				if (argn >= argc)
					usage();
				ss = symstr_create(gstab, argv[argn]);
				equeue_insert(event_queue, ss, NULL); 
				break;
			case 'd':
				do_dump = 1;
				break;
			case 'i':	/* run the interactive cmdline */
				interactive = 1;
				break;
			case 't':	/* trace events as they happen */
				trace_flags |= (TRACE_EVENTS |
						TRACE_QUEUE |
						TRACE_HISTORY);
				break;
			default:
				usage();
			}
		} else {
			break;
		}
	}
	
	/* Parse the input file. */

	while (argn < argc) {
		sc = scan_open(argv[argn]);
		if (sc != NULL) {
			two_iota(sc);
			scan_close(sc);
		} else {
			fprintf(stderr, "WARNING: Couldn't open %s\n", argv[argn]);
		}
		argn++;
	}

	if (do_dump)
		debug_dump_all();

	if (interactive)
		cmdline();
	else
		ii_engine_run(event_table, event_queue, event_hist);

	/* symbol_table_free(gstab); */
	exit(0);
}

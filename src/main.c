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
 * main.c
 * 2iota - Reference interpreter for the 2Iota programming language.
 * $Id: main.c 54 2004-04-23 22:51:09Z catseye $
 */

#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <sysexits.h>
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

struct symbol_table	*gstab;	/* general symbol table */
struct symbol_table	*astab;	/* symbol table for alphabets */
struct event_table	*etab;	/* global table of events */
struct equeue		*eq;	/* global event queue */
struct ehist		*eh;	/* global event history log */

int			interactive = 0;

/* FUNCTIONS */

int
main(int argc, char **argv)
{
	struct scan_st *sc;
	struct symstr *ss;
	int ch;
	int do_dump = 0;

#ifdef DETECT_LEAKS
	GC_find_leak = 1;
#endif

	/* Initialize / allocate globals. */
	
	gstab = symbol_table_new();
	astab = symbol_table_new();
	etab = event_table_new();
	eq = equeue_new();
	eh = ehist_new();

	/* Parse arguments. */

	while ((ch = getopt(argc, argv, "c:dit")) != -1) {
		switch(ch) {
		case 'c':	/* cause initial event */
			ss = symstr_create(gstab, optarg);
			equeue_insert(eq, ss, NULL); 
			break;
		case 'd':
			do_dump = 1;
			break;
		case 'i':	/* run the interactive cmdline */
			interactive = 1;
			break;
		case 't':	/* trace events as they happen */
			trace_flags |=
			    (TRACE_EVENTS | TRACE_QUEUE | TRACE_HISTORY);
			break;
		case '?':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;
	
	/* Parse the input file. */

	while (argc > 0) {
		sc = scan_open(argv[0]);
		two_iota(sc);
		scan_close(sc);
		argc--;
		argv++;
	}

	if (do_dump)
		debug_dump_all();

	if (interactive)
		cmdline();
	else
		ii_engine_run(etab, eq, eh);

	/* symbol_table_free(gstab); */
	exit(0);
}

void
usage(void)
{
	fprintf(stderr, "usage: 2iota [-dit] [-c event] srcfile ...\n");
	exit(EX_USAGE);
}

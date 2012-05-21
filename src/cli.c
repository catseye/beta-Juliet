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
 * cli.c
 * Command-line interface for 2iota.
 * $Id: cli.c 518 2010-04-28 17:48:38Z cpressey $
 */

#include <ctype.h>			/* XXX implement scan.c on strings, eh? */
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "symbol.h"
#include "symstr.h"
#include "event.h"
#include "equeue.h"

#include "engine.h"
#include "cli.h"
#include "debug.h"
#include "2iota.h"

int		tokenize_args(char *);	/* XXX implement scan.c on strings, eh? */

static void
signal_handler(int signo)
{
	caught_signal = signo;
}

void
cmdline(void)
{
	char cmd[256];
	int done = 0;
	
	while (!done) {
		printf("2iota> ");
		fflush(stdout);
		fgets(cmd, 256, stdin);
		if (feof(stdin)) {
			printf("\n");
			fflush(stdout);
			done = 1;
		} else
			done = do_cmd(cmd);
	}
}

int
do_cmd(char *cmd)
{
	char *rcmd, *args;
	struct symstr *ss;

	if (!parse_cmd(cmd, &rcmd, &args)) {
		printf("Indecipherable command `%s'\n", cmd);
		help();
		return(0);
	}

	if (!strcmp(rcmd, "cause")) {
		ss = symstr_create(gstab, args);
		equeue_insert(event_queue, ss, NULL);
		return(0);
	}

	if (!strcmp(rcmd, "match")) {
		int result;
		struct event *e;

		ss = symstr_create(gstab, args);
		for (e = event_table->head; e != NULL; e = e->next) {
			symstr_dump(ss);
			fprintf(stderr, " binds to ");
			symstr_dump(e->name);
			fprintf(stderr, "?\n --> ");
			symbol_table_unbind(e->params);
			result = symstr_bind(ss, e->name, e->params);
			fprintf(stderr, "%s\n", symstr_bind_result[result]);
			if (result == SYMSTR_BIND_OK)
				event_dump(e);
		}
		symstr_free(ss);
		return(0);
	}

	if (!strcmp(rcmd, "trace")) {
		int what = 0;

		if ((what = tokenize_args(args)) == 0)
			help();
		else
			trace_flags |= what;

		return(0);
	}

	if (!strcmp(rcmd, "notrace")) {
		int what = 0;

		if ((what = tokenize_args(args)) == 0)
			help();
		else
			trace_flags &= ~what;

		return(0);
	}

	if (!strcmp(rcmd, "step")) {
		struct equeue *neq;
		
		neq = equeue_new();
		ii_engine_step(event_table, event_queue,
			       event_hist, neq);
		equeue_transfer(event_queue, neq);

		return(0);
	}

	if (!strcmp(rcmd, "walk")) {
		caught_signal = 0;
		signal(SIGINT, signal_handler);
		ii_engine_walk(event_table, event_queue, event_hist);
		signal(SIGINT, SIG_DFL);
		return(0);
	}

	if (!strcmp(rcmd, "run")) {
		caught_signal = 0;
		signal(SIGINT, signal_handler);
		ii_engine_run(event_table, event_queue, event_hist);
		signal(SIGINT, SIG_DFL);
		return(0);
	}

	if (!strcmp(rcmd, "dump")) {
		debug_dump(args);
		return(0);
	}
	
#ifdef DETECT_LEAKS
	if (!strcmp(rcmd, "leaks")) {
		CHECK_LEAKS();
		return(0);
	}
#endif

	if (!strcmp(rcmd, "quit")) {
		return(1);
	}

	printf("Unknown command `%s'\n", rcmd);
	help();
	return(0);
}

char *null_string = NULL;

int
parse_cmd(char *cmd, char **rcmd, char **args)
{
	int i = 0, j = 0;

	if (null_string == NULL) {
		null_string = malloc(1);
                if (null_string == NULL)
                        errx(1, "Could not allocate space for null string");
                strcpy(null_string, "");
	}

	*rcmd = null_string;
	*args = null_string;

	i = strlen(cmd) - 1;
	while(isspace((int)cmd[i])) {
		cmd[i] = '\0';
		i--;
	}

	i = 0;
	while (isspace((int)cmd[i]) && cmd[i] != '\0')
		i++;
	if (cmd[i] == '\0')
		return(0);
	*rcmd = &cmd[i];
	while (!isspace((int)cmd[i]) && cmd[i] != '\0')
		i++;
	j = i;
	if (cmd[i] != '\0') {
		while (isspace((int)cmd[i]) && cmd[i] != '\0')
			i++;
		if (cmd[i] != '\0')
			*args = &cmd[i];
	}
	cmd[j] = '\0';
	return(1);
}

int
tokenize_args(char *args)
{
	int what = 0;

	if (!strcmp(args, "events"))
		what = TRACE_EVENTS;
	if (!strcmp(args, "history"))
		what = TRACE_HISTORY;
	if (!strcmp(args, "queue"))
		what = TRACE_QUEUE;
	if (!strcmp(args, "all"))
		what = TRACE_EVENTS | TRACE_HISTORY | TRACE_QUEUE;

	return(what);
}

void
help(void)
{
	printf("Summary of 2iota CLI Commands\n");
	printf("-----------------------------\n");
	printf("run               run program from current state\n");
	printf("walk              run through event queue only once\n");
	printf("step              fire only the next pending event\n");
	printf("cause <event>     add given event name to event queue\n");
	printf("match <event>     list events matching given event name\n");
	printf("dump <what>       dump program element to stderr\n");
	printf("                  what = symbols|alphabets|events|queue|history|all\n");
	printf("trace <what>      show things as they happen\n");
	printf("notrace <what>    don't show things as they happen\n");
	printf("                  what = events|queue|history|all\n");
	printf("clear <what>      remove things from program state\n");
	printf("                  what = queue|history|all\n");
#ifdef DETECT_LEAKS
	printf("leaks             check for memory leaks\n");
#endif
	printf("quit              leave 2iota interpreter\n");
	fflush(stdout);
}

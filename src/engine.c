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
 * engine.c
 * Event processing engine for 2Iota.
 * $Id: engine.c 518 2010-04-28 17:48:38Z cpressey $
 */

#include <stdio.h>
#include <signal.h>

#include "equeue.h"
#include "event.h"
#include "symstr.h"	/* will be debug.h */
#include "engine.h"

volatile sig_atomic_t caught_signal;

/*
 * Keeps running events until the event queue is empty.
 * Returns the number of events removed from event queue.
 */
int
ii_engine_run(struct event_table *etab, struct equeue *eq, struct ehist *eh)
{
	int did_fire, fired = 0;

	do {
		did_fire = ii_engine_walk(etab, eq, eh);
		fired += did_fire;
	} while (did_fire > 0 && !caught_signal);
	
	return(fired);
}

/*
 * Runs all events in the event queue.
 * Returns the number of events removed from event queue.
 */
int
ii_engine_walk(struct event_table *etab, struct equeue *eq, struct ehist *eh)
{
	struct equeue *neq;
	int did_fire, fired = 0;

	neq = equeue_new();

	do {
		did_fire = ii_engine_step(etab, eq, eh, neq);
		fired += did_fire;
	} while (did_fire && !caught_signal);

	/* move all equeue_entry's from neq to eq and free neq */
	equeue_transfer(eq, neq);

	return(fired);
}

/*
 * Runs a single event from in the event queue.
 * Returns the number of events removed from the queue (0 or 1).
 */
int
ii_engine_step(struct event_table *etab, struct equeue *eq, struct ehist *eh,
	       struct equeue *neq)
{
	struct equeue_entry *eqe = NULL;
	struct event *e = NULL;

	if ((eqe = equeue_remove(eq)) != NULL) {
		e = event_find(etab, eqe->name);
		if (e == NULL) {
			/* if noisy... */
			fprintf(stderr, "Error: No event matching ");
			symstr_dump(eqe->name);
			fprintf(stderr, " found\n");
		} else {
			etime_sleep(&eqe->delay);
			equeue_time_passes(eq, &eqe->delay);
			ehist_update(eh, eqe->name, NULL);
			event_happen(e, neq, eh);
		}
		equeue_entry_free(eqe);
		return(1);
	} else {
		/* if noisy... */
		fprintf(stderr, "No pending events.\n");
		return(0);
	}
}

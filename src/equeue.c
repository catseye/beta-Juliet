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
 * equeue.c
 * Event queue housekeeping and manipulation for 2Iota.
 * $Id: equeue.c 54 2004-04-23 22:51:09Z catseye $
 */

#include <stdio.h>
#include <stdlib.h>

#include "equeue.h"
#include "symstr.h"
#include "etime.h"
#include "ehist.h"
#include "debug.h"
#include "2iota.h"

struct equeue *
equeue_new(void)
{
	struct equeue *eq;

	MALLOC(eq, equeue, "event queue");

	eq->head = NULL;
	return(eq);
}

/*
 * If delay == NULL, use a delay of zero.
 */
struct equeue_entry *
equeue_insert(struct equeue *eq, struct symstr *ss, struct etime *delay)
{
	struct equeue_entry *eqe;
	struct etime et;

	if (delay == NULL)
		et = etime_zero;
	else
		et = *delay;

	MALLOC(eqe, equeue_entry, "event queue entry");

	eqe->name = ss;		/* Note that this is NOT dup'ed. */
	eqe->delay = et;

	eqe->next = eq->head;
	eqe->prev = NULL;
	if (eq->head != NULL)
		eq->head->prev = eqe;

	eq->head = eqe;

	debug_trace(TRACE_QUEUE, "Inserted %S with delay %T", ss, &et);
	
	return(eqe);
}

/*
 * Remove the next-up entry from the queue.  NULL is returned if the queue is empty.
 */
struct equeue_entry *
equeue_remove(struct equeue *eq)
{
	struct equeue_entry *eqe, *soonest;

	if (eq->head == NULL)
		return(NULL);

	soonest = eq->head;
	for (eqe = eq->head; eqe != NULL; eqe = eqe->next) {
		if (etime_compare(&eqe->delay, &soonest->delay) < 0)
			soonest = eqe;
	}

	/* unlink soonest */
	
	if (soonest->prev == NULL)
		eq->head = soonest->next;
	else
		soonest->prev->next = soonest->next;
	if (soonest->next != NULL)
		soonest->next->prev = soonest->prev;

	soonest->next = NULL;
	soonest->prev = NULL;

	debug_trace(TRACE_QUEUE, "Removed %S with delay %T",
	    soonest->name, &soonest->delay);

	return(soonest);
}

void
equeue_time_passes(struct equeue *eq, struct etime *dur)
{
	struct equeue_entry *eqe;

	for (eqe = eq->head; eqe != NULL; eqe = eqe->next) {
		etime_delta(&eqe->delay, dur, -1);
	}
}

/*
 * Transfer all equeue entries from the event queue from_eq
 * to the event queue to_eq.  NOTE: from_eq is then deallocated.
 */
void
equeue_transfer(struct equeue *to_eq, struct equeue *from_eq)
{
	struct equeue_entry *eqe;

	if (to_eq->head == NULL) {
		to_eq->head = from_eq->head;
	} else {
		eqe = to_eq->head;
		while (eqe->next != NULL)
			eqe = eqe->next;
		eqe->next = from_eq->head;
		if (from_eq->head != NULL)
			from_eq->head->prev = eqe->next;
	}

	from_eq->head = NULL;
	equeue_free(from_eq);
}

void
equeue_free(struct equeue *eq)
{
	struct equeue_entry *eqe, *neqe;

	if (eq == NULL)
		return;

	eqe = eq->head;
	while (eqe != NULL) {
		neqe = eqe->next;
		equeue_entry_free(eqe);
		eqe = neqe;
	}
	
	free(eq);
}

void
equeue_entry_free(struct equeue_entry *eqe)
{
	if (eqe == NULL)
		return;
	symstr_free(eqe->name);
	free(eqe);
}

void
equeue_dump(struct equeue *eq)
{
	struct equeue_entry *eqe;
	
	for (eqe = eq->head; eqe != NULL; eqe = eqe->next) {
		symstr_dump(eqe->name);
		fprintf(stderr, " in ");
		etime_dump(&eqe->delay);
		fprintf(stderr, "\n");
	}
}

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
 * event.h
 * Event housekeeping and manipulation for 2Iota.
 * $Id: event.c 518 2010-04-28 17:48:38Z cpressey $
 */

#include <stdio.h>
#include <stdlib.h>

#include "event.h"
#include "symbol.h"
#include "symstr.h"
#include "ehist.h"
#include "etime.h"
#include "debug.h"
#include "2iota.h"

struct event_table *
event_table_new(void)
{
	struct event_table *etab;

	MALLOC(etab, event_table, "event table");

	etab->head = NULL;
	return(etab);
}

struct event *
event_new(struct event_table *etab)
{
	struct event *e;

	MALLOC(e, event, "event");
	
	e->next = etab->head;
	e->name = symstr_new();
	e->head = NULL;
	e->params = symbol_table_new();
	etab->head = e;
	return(e);
}

struct consequence *
event_consequence_append(struct event *cause,
			 struct symstr *effect,
			 struct etime *delay)
{
	struct consequence *c;

	MALLOC(c, consequence, "consequence");

	c->next = cause->head;
	c->effect = effect;
	if (delay == NULL)
		c->delay = etime_zero;
	else
		c->delay = *delay;

	cause->head = c;

	return(c);
}

struct condition *
consequence_condition_append(struct consequence *c,
			     struct symstr *earlier, struct symstr *later)
{
	struct condition *cond;

	MALLOC(cond, condition, "condition");

	cond->next = c->head;
	cond->earlier = earlier;
	cond->later = later;

	c->head = cond;

	return(cond);
}

/*
 * Given a name of an event, find the matching event with the most
 * specific name in the given event table.
 * Returns NULL if no matching event was found.
 */
struct event *
event_find(struct event_table *etab, struct symstr *ss)
{
	int result;
	struct event *e;
		
	struct event *cand = NULL;
	int rank, order;
	int best_rank = 3;
	int best_order = 65535;		/* XXX well, infinite really */

	for (e = etab->head; e != NULL; e = e->next) {
		symbol_table_unbind(e->params);
		result = symstr_bind(ss, e->name, e->params);
		if (result == SYMSTR_BIND_OK) {
			symstr_specificity(e->name, &rank, &order);
			debug_trace(TRACE_EVENTS, "Found %S, rank=%d, order=%d",
			    e->name, rank, order);
			if (rank < best_rank ||
			    (rank == best_rank && order < best_order)) {
				best_rank = rank;
				best_order = order;
				cand = e;
			}
		}
	}

	if (cand != NULL)
		debug_trace(TRACE_EVENTS, "Best Match %S, rank=%d, order=%d",
			    cand->name, best_rank, best_order);

	return(cand);
}

int
event_happen(struct event *e, struct equeue *eq, struct ehist *eh)
{
	struct consequence *c;
	struct condition *cond;
	struct symstr *ss;
	int result = 0;
	int condok;

	debug_trace(TRACE_EVENTS, "Triggering %S:", e->name);
	for (c = e->head; c != NULL; c = c->next) {
		ss = symstr_substitute(c->effect, e->params);
		debug_trace(TRACE_EVENTS, "Consequence %S:", ss);
		condok = 1;
		for (cond = c->head; cond != NULL; cond = cond->next) {
			/* TODO: subst these too */
			condok = ehist_was_later(eh, cond->later, cond->earlier);
			debug_trace(TRACE_EVENTS,
			    "Condition: %S > %S = %B",
			    cond->later, cond->earlier, condok);
			if (!condok)
				break;
		}
		if (condok) {
			equeue_insert(eq, ss, &c->delay);
			result++;
		} else {
			symstr_free(ss);
		}
	}
	
	return(result);
}

void
event_dump(struct event *e)
{
	struct consequence *c;
	struct condition *cond;

	fprintf(stderr, "event ");
	if (e == NULL) {
		fprintf(stderr, "*NULL*\n");
		return;
	}
	symstr_dump(e->name);
	fprintf(stderr, "\n");

	symbol_table_dump(1, e->params);
	for (c = e->head; c != NULL; c = c->next) {
		fprintf(stderr, "\tcauses ");
		symstr_dump(c->effect);
		fprintf(stderr, " after ");
		etime_dump(&c->delay);
		fprintf(stderr, "\n");
		for (cond = c->head; cond != NULL; cond = cond->next) {
			fprintf(stderr, "\t\twhen ");
			symstr_dump(cond->later);
			fprintf(stderr, " > ");
			symstr_dump(cond->earlier);
			fprintf(stderr, "\n");
		}
	}
}

void
event_table_dump(struct event_table *etab)
{
	struct event *e;

	for (e = etab->head; e != NULL; e = e->next)
		event_dump(e);
}

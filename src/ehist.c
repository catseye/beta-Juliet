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
 * ehist.c
 * Event history housekeeping and manipulation for 2Iota.
 * $Id: ehist.c 54 2004-04-23 22:51:09Z catseye $
 */

#include <stdio.h>
#include <stdlib.h>

#include "ehist.h"
#include "symstr.h"
#include "etime.h"
#include "debug.h"
#include "2iota.h"

struct ehist *
ehist_new(void)
{
	struct ehist *eh;

	MALLOC(eh, ehist, "event history");

	eh->head = NULL;
	return(eh);
}

struct ehist_entry *
ehist_find(struct ehist *eh, struct symstr *ss)
{
	struct ehist_entry *ehe;

	for (ehe = eh->head; ehe != NULL; ehe = ehe->next) {
		if (symstr_bind(ss, ehe->name, NULL) == SYMSTR_BIND_OK)
			break;
	}
	
	return(ehe);
}

/*
 * Passing NULL as the third argument means use the current time.
 */
struct ehist_entry *
ehist_update(struct ehist *eh, struct symstr *ss, struct etime *et)
{
	struct ehist_entry *ehe;
	struct etime pet;

	if (et == NULL)
		etime_now(&pet);
	else
		pet = *et;

	if ((ehe = ehist_find(eh, ss)) == NULL) {
		debug_trace(TRACE_HISTORY, "%S first fired at %T", ss, &pet);
		
		MALLOC(ehe, ehist_entry, "event history entry");

		ehe->next = eh->head;
		eh->head = ehe;

		ehe->name = symstr_dup(ss);
	} else {
		debug_trace(TRACE_HISTORY, "%S last fired at %T", ehe->name, &pet);
	}

	ehe->when = pet;
	return(ehe);
}

int
ehist_was_later(struct ehist *eh, struct symstr *later, struct symstr *earlier)
{
	struct ehist_entry *ehe_later, *ehe_earlier;

	ehe_earlier = ehist_find(eh, earlier);
	ehe_later = ehist_find(eh, later);
	if (ehe_earlier == NULL && ehe_later == NULL) {
		/*
		 * Neither event is in the history.
		 * Since the question is technically,
		 * "Do we *know* that 'later' occurred
		 * more recently than 'earlier'?",
		 * we return an answe of 'no' here.
		 */
		return(0);
	} else if (ehe_earlier == NULL && ehe_later != NULL) {
		/*
		 * Later is in history, but earlier isn't -
		 * so we know that later happened more recently.
		 */
		return(1);
	} else if (ehe_earlier != NULL && ehe_later == NULL) {
		/*
		 * Earlier is in history, but later isn't -
		 * so we know that earlier happened more recently.
		 */
		return(0);
	} else {
		/* They're both in history. Compare their etimes. */
		int result;
		
		result = etime_compare(&ehe_earlier->when, &ehe_later->when);
		return(result < 0 ? 1 : 0);
	}
}

void
ehist_dump(struct ehist *eh)
{
	struct ehist_entry *ehe;
	
	for (ehe = eh->head; ehe != NULL; ehe = ehe->next) {
		symstr_dump(ehe->name);
		fprintf(stderr, " : ");
		etime_dump(&ehe->when);
		fprintf(stderr, "\n");
	}
}

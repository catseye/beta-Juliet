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
 * alphabet.c
 * Alphabet housekeeping and manipulation for 2Iota.
 * $Id: alphabet.c 518 2010-04-28 17:48:38Z cpressey $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"
#include "alphabet.h"
#include "2iota.h"

struct alphabet *
alphabet_new(void)
{
	struct alphabet *alpha;

	MALLOC(alpha, alphabet, "alphabet");
	
	alpha->head = NULL;
	alpha->tail = NULL;
	return(alpha);
}

void
alphabet_append(struct alphabet *alpha, struct symbol *sym)
{
	struct alpha_entry *entry;
	
	MALLOC(entry, alpha_entry, "alphabet entry");
	
	entry->sym = sym;
	entry->next = NULL;
	entry->prev = alpha->tail;
	if (alpha->tail != NULL)
		alpha->tail->next = entry;
	if (alpha->head == NULL)
		alpha->head = entry;
	alpha->tail = entry;
}
	
void
alphabet_free(struct alphabet *alpha)
{
	alpha = alpha;	/* XXX */
}

struct alpha_entry *
alphabet_lookup(struct alphabet *alpha, struct symbol *sym)
{
	struct alpha_entry *entry;
	
	for (entry = alpha->head; entry != NULL; entry = entry->next) {
		if (entry->sym == sym)
			return entry;
	}

	return(NULL);
}

struct symbol *
alphabet_first(struct alphabet *alpha)
{
	return(alpha->head->sym);
}

struct symbol *
alphabet_last(struct alphabet *alpha)
{
	return(alpha->tail->sym);
}

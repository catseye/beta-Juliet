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
 * symbol.c
 * Symbol and symbol table housekeeping and manipulation for 2Iota.
 * $Id: symbol.c 518 2010-04-28 17:48:38Z cpressey $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "symbol.h"
#include "alphabet.h"
#include "2iota.h"

struct symbol_table *
symbol_table_new(void)
{
	struct symbol_table *stab;

	MALLOC(stab, symbol_table, "symbol table");
	stab->head = NULL;
	return(stab);
}

/*
 * Intended to be called on the symbol tables of
 * events (representing their local parameters,)
 * and causes all the local parameters to become
 * unbound.
 */
void
symbol_table_unbind(struct symbol_table *stab)
{
	struct symbol *sym;
	
	for (sym = stab->head; sym != NULL; sym = sym->next) {
		symstr_free(sym->data);
		sym->data = NULL;
	}
}

/* assumed: preceding call to symbol_lookup failed. */
struct symbol *
symbol_define(struct symbol_table *stab, char *token, int type)
{
	struct symbol *new_sym;

	MALLOC(new_sym, symbol, "symbol");
        new_sym->token = malloc(strlen(token) + 1);
	if (new_sym->token == NULL)
		errx(1, "Could not allocate symbol lexeme");
        strcpy(new_sym->token, token);
	new_sym->type = type;
	new_sym->next = stab->head;
	stab->head = new_sym;
	new_sym->alpha = NULL;
	new_sym->data = NULL;
	return(new_sym);
}

struct symbol *
symbol_lookup(struct symbol_table *stab, char *s)
{
	struct symbol *sym;

	for (sym = stab->head; sym != NULL; sym = sym->next)
		if (!strcmp(s, sym->token))
			return(sym);
	return(NULL);
}

struct symbol *
symbol_next(struct symbol *sym, struct alphabet *alpha)
{
	struct alpha_entry *entry;
	
	if ((entry = alphabet_lookup(alpha, sym)) != NULL)
		if (entry->next != NULL)
			return(entry->next->sym);
	return(NULL);
}

struct symbol *
symbol_prev(struct symbol *sym, struct alphabet *alpha)
{
	struct alpha_entry *entry;
	
	if ((entry = alphabet_lookup(alpha, sym)) != NULL)
		if (entry->prev != NULL)
			return(entry->prev->sym);
	return(NULL);
}

void
symbol_table_dump(int indent, struct symbol_table *stab)
{
	struct symbol *sym;

	for (sym = stab->head; sym != NULL; sym = sym->next)
		symbol_dump(indent, sym);
}

void
symbol_print(struct symbol *sym)
{
	if (sym != NULL)
		fprintf(stderr, "%s", sym->token);
	else
		fprintf(stderr, "*SYMNULL*");
}

void
symbol_dump(int indent, struct symbol *sym)
{
	const char *ty;
	int i;

	for (i = 0; i < indent; i++)
		fprintf(stderr, "\t");

	if (sym == NULL) {
		fprintf(stderr, "*NULLSYM*\n");
		return;
	}

	switch (sym->type) {
	case SYM_TYPE_LITERAL:
		ty = "literal";
		break;
	case SYM_TYPE_ALPHABET:
		ty = "alphabet";
		break;
	case SYM_TYPE_SYMBOL:
		ty = "symbol";
		break;
	case SYM_TYPE_PARAMETER:
		ty = "parameter";
		break;
	default:
		ty = "?";
		break;
	}

	fprintf(stderr, "%-30s %-10s", sym->token, ty);
	if (sym->type == SYM_TYPE_ALPHABET) {
		struct alpha_entry *entry;

		fprintf(stderr, " ->\n");
		for (entry = sym->alpha->head; entry != NULL; entry = entry->next) {
			symbol_dump(indent + 1, entry->sym);
		}
	} else if (sym->type == SYM_TYPE_PARAMETER) {
		fprintf(stderr, " = ");
		symstr_dump(sym->data);
		fprintf(stderr, "\n");
	} else {
		fprintf(stderr, "\n");
	}
}

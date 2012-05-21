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
 * symstr.c
 * Symbol strings for 2Iota.
 * $Id: symstr.c 54 2004-04-23 22:51:09Z catseye $
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"
#include "alphabet.h"
#include "symstr.h"
#include "2iota.h"

char *symstr_bind_result[] = {
	"Bound OK",
	"Mismatch between Literals",
	"String too Short",
	"Pattern too Short",
	"Bad String!",
	"Bad Pattern!",
	"Symbol outside Alphabet",
	"*NOT IMPLEMENTED*"
};

struct symstr *
symstr_new(void)
{
	struct symstr *ss;

	MALLOC(ss, symstr, "symbol string");

	ss->head = NULL;
	ss->tail = NULL;

	return(ss);
}

/*
 * Creates a literal symstr from a plain string.
 * Not used by the scanner/parser.
 */
struct symstr *
symstr_create(struct symbol_table *stab, char *s)
{
	struct symstr *ss;
	struct symbol *sym;
	char word[128];
	int i, j;

	i = 0;
	ss = symstr_new();
	while (s[i] != '\0') {
		while (isspace(s[i]) && s[i] != '\0')
			i++;
		if (s[i] == '\0')
			break;
		j = 0;
		while (!isspace(s[i]) && s[i] != '\0' && j < 128)
			word[j++] = s[i++];
		word[j] = '\0';
		if ((sym = symbol_lookup(stab, word)) == NULL)
			sym = symbol_define(stab, word, SYM_TYPE_LITERAL);
		symstr_append(ss, sym, NULL, 0, SYMSTR_OP_NOP);
	}

	return(ss);
}

struct symstr *
symstr_dup(struct symstr *ss)
{
	struct symstr *new_ss;

	new_ss = symstr_new();
	symstr_append_symstr(new_ss, ss);

	return(new_ss);
}

void
symstr_free(struct symstr *ss)
{
	struct symstr_component *sc, *nsc;

	if (ss == NULL)
		return;

	/*
	 * Note that this does not free the symbols or alphabets themselves!
	 * We generally want those to stick around indefinately.
	 */
	sc = ss->head;
	while (sc != NULL) {
		nsc = sc->next;
		symstr_component_free(sc);
		sc = nsc;
	}
	FREE(ss, "symbol string");
}

void
symstr_component_free(struct symstr_component *sc)
{
	if (sc == NULL)
		return;

	symstr_free(sc->fmode);
	FREE(sc, "symbol string component");
}

struct symstr_component	*
symstr_component_new(struct symbol *sym, struct symbol *alphasym,
		     int many, int op)
{
	struct symstr_component *sc;

	MALLOC(sc, symstr_component, "symbol string component");

	sc->sym = sym;
	sc->alphasym = alphasym;
	sc->many = many;
	sc->next = NULL;
	sc->prev = NULL;
	sc->op = op;
	sc->fmode = NULL;

	return(sc);
}

struct symstr_component	*
symstr_append(struct symstr *ss, struct symbol *sym,
		struct symbol *alphasym, int many, int op)
{
	struct symstr_component *sc;

	sc = symstr_component_new(sym, alphasym, many, op);
	if (ss->head == NULL) {
		ss->head = sc;
		ss->tail = sc;
	} else {
		sc->prev = ss->tail;
		ss->tail->next = sc;
		ss->tail = sc;
	}
	return(sc);
}

struct symstr_component	*
symstr_prepend(struct symstr *ss, struct symbol *sym,
		struct symbol *alphasym, int many, int op)
{
	struct symstr_component *sc;

	sc = symstr_component_new(sym, alphasym, many, op);
	if (ss->head == NULL) {
		ss->head = sc;
		ss->tail = sc;
	} else {
		sc->next = ss->head;
		ss->head->prev = sc;
		ss->head = sc;
	}
	return(sc);
}

void
symstr_append_symstr(struct symstr *dest, struct symstr *src)
{
	struct symstr_component *sc;

	for (sc = src->head; sc != NULL; sc = sc->next)
		symstr_append(dest, sc->sym, NULL, 0, SYMSTR_OP_NOP);
}

void
symstr_remove(struct symstr *ss, struct symstr_component *sc)
{
	if (sc->prev != NULL)
		sc->prev->next = sc->next;
	if (sc->next != NULL)
		sc->next->prev = sc->prev;
	if (ss->head == sc)
		ss->head = sc->next;
	if (ss->tail == sc)
		ss->tail = sc->prev;
	sc->next = NULL;
	sc->prev = NULL;
	symstr_component_free(sc);
}

/*
 * Determine the specificity of the given symstr.
 * rank is the overall specificity rank:
 *	0:	only literals (most specific)
 *	1:	some single-symbol free variables
 *	2:	some multi-symbol free variables (least specific)
 * order is the number of free variables as apropos to the rank.
 */
void
symstr_specificity(struct symstr *ss, int *rank, int *order)
{
	struct symstr_component *sc;
	int count_single_freevars = 0;
	int count_multi_freevars = 0;
	
	*rank = 0;
	*order = 0;
	
	for (sc = ss->head; sc != NULL; sc = sc->next) {
		if (sc->alphasym != NULL) {
			if (sc->many > 0) {
				*rank = 2;
				count_multi_freevars++;
			} else {
				if (*rank == 0)
					*rank = 1;
				count_single_freevars++;
			}
		}
	}

	switch (*rank) {
	case 1:
		*order = count_single_freevars;
		break;
	case 2:
		*order = count_multi_freevars;
		break;
	}
}

struct symstr *
symstr_substitute(struct symstr *pat, struct symbol_table *stab)
{
	struct symstr *dest;
	struct symstr_component *patc;

	dest = symstr_new();

	for (patc = pat->head; patc != NULL; patc = patc->next) {
		if (patc->sym->type == SYM_TYPE_PARAMETER) {
			/* variable? insert its bound value */
			if (patc->sym->data != NULL) {
				symstr_component_substitute(patc, dest,
				    patc->sym->data);
			} else {
				fprintf(stderr, "WHACKY: symbol is not bound?\n");
			}
		} else {
			/* literal? just copy it */
			symstr_append(dest, patc->sym, NULL, 0, SYMSTR_OP_NOP);
		}
	}
	
	return(dest);
}

void
symstr_component_substitute(struct symstr_component *bc, struct symstr *dest,
			    struct symstr *boundval)
{
	int ok = 0;
	struct symstr *res = NULL;

	while (!ok) {
		/* Apply bc->op. */
		if (bc->op == SYMSTR_OP_NOP) {
			/*
			 * XXX this could be the name of a local
			 * parameter, or it could be a literal.
			 * If it's a parameter, should use boundval.
			 * Assume it's a literal, for now.
			 */
			res = symstr_new();
			symstr_append(res, bc->sym, NULL, 0, SYMSTR_OP_NOP);
			ok = 1;
		} else if (bc->op == SYMSTR_OP_SUCC) {
			res = symstr_succ(boundval, bc->sym->alpha, &ok);
		} else if (bc->op == SYMSTR_OP_PRED) {
			res = symstr_pred(boundval, bc->sym->alpha, &ok);
		} else if (bc->op == SYMSTR_OP_NEXT) {
			res = symstr_next(boundval, bc->sym->alpha, &ok);
		} else if (bc->op == SYMSTR_OP_PREV) {
			res = symstr_prev(boundval, bc->sym->alpha, &ok);
		} else {
			/* Complain severely. */
			fprintf(stderr, "Not a legal op\n");
			ok = 0;
			res = NULL;
		}

		if (ok) {
			symstr_append_symstr(dest, res);
		} else {
			if (bc->fmode != NULL) {
				bc = bc->fmode->head;
			} else {
				/* Complain severely. */
				fprintf(stderr, "Failure not handled\n");
			}
		}
		symstr_free(res);
	}
}
			
int
symstr_bind(struct symstr *str, struct symstr *pat, struct symbol_table *stab)
{
	struct symstr_component *strc, *patc;
	struct alphabet *alpha;
	struct symbol *tsym;

	strc = str->head;
	patc = pat->head;

	for (;;) {
		if (strc == NULL && patc == NULL)
			return(SYMSTR_BIND_OK);
		if (strc == NULL)
			return(SYMSTR_BIND_STRING_TOO_SHORT);
		if (patc == NULL)
			return(SYMSTR_BIND_PATTERN_TOO_SHORT);
		/* Some bogosity checking - str should not be a pattern! */
		if (strc->alphasym != NULL) {
			return(SYMSTR_BIND_BAD_STRING);
		}
		/* Compare components. */
		if (patc->alphasym == NULL) {
			/* The literal case. */
			if (strc->sym == patc->sym) {
				strc = strc->next;
				patc = patc->next;
				continue;
			} else {
				return(SYMSTR_BIND_LITERAL_MISMATCH);
			}
		} else {
			/* The 'free variable' case. */
			if (stab == NULL) {
				/*
				 * No symbol table was passed, so we really
				 * can't bind anything - assume that this
				 * only works with a literal (no-free-vars)
				 * pattern.
				 */
				return(SYMSTR_BIND_BAD_PATTERN);
			}

			alpha = patc->alphasym->alpha;

			/* Check to see that the symbol is in the given alphabet. */
			if (alphabet_lookup(alpha, strc->sym) == NULL)
				return(SYMSTR_BIND_OUTSIDE_ALPHABET);

			/* Bind it in the given symbol table. */
			tsym = symbol_lookup(stab, patc->sym->token);
			if (tsym != NULL) {
				tsym->data = symstr_new();
				if (patc->many) {
					/* while it is in the alphabet */
					while (strc != NULL &&
					    alphabet_lookup(alpha, strc->sym) != NULL) {
						symstr_append(tsym->data, strc->sym,
						    NULL, 0, SYMSTR_OP_NOP);
						strc = strc->next;
					}
				} else {
					/* just once */
					symstr_append(tsym->data, strc->sym,
					    NULL, 0, SYMSTR_OP_NOP);
					strc = strc->next;
				}
				patc = patc->next;
			} else {
				/* Bogosity!  It *should* be there. */
				return(SYMSTR_BIND_BAD_PATTERN);
			}
		}
	}
}

struct symstr *
symstr_succ(struct symstr *ss, struct alphabet *alpha, int *ok)
{
	struct symstr *new_ss;
	struct symstr_component *sc;
	struct symbol *sym;

	new_ss = symstr_dup(ss);

	*ok = 1;
	sc = new_ss->tail;
	for (;;) {
		sym = symbol_next(sc->sym, alpha);
		if (sym != NULL) {
			sc->sym = sym;
			break;
		} else {
			sc->sym = alphabet_first(alpha);
			sc = sc->prev;
			if (sc == NULL) {
				symstr_prepend(new_ss, alphabet_first(alpha),
				    NULL, 0, SYMSTR_OP_NOP);
				break;
			}
		}
	}

	return(new_ss);
}

struct symstr *
symstr_pred(struct symstr *ss, struct alphabet *alpha, int *ok)
{
	struct symstr *new_ss;
	struct symstr_component *sc;
	struct symbol *sym;

	new_ss = symstr_dup(ss);

	*ok = 1;
	sc = new_ss->tail;
	for (;;) {
		sym = symbol_prev(sc->sym, alpha);
		if (sym != NULL) {
			/* There's a prev symbol in the alphabet - use it. */
			sc->sym = sym;
			break;
		} else {
			/* We can't find the prev symbol in the alphabet. */
			if (sc->prev == NULL) {
				/* We are at the leftmost symbol. */
				if (sc->next == NULL) {
					/* Actually, it's the ONLY symbol. */
					*ok = 0;
					symstr_free(new_ss);
					new_ss = symstr_dup(ss);
					break;
				} else {
					/* Erase this symbol. */
					symstr_remove(new_ss, sc);
					break;
				}
			}
			/* Wrap around. */
			sc->sym = alphabet_last(alpha);
			sc = sc->prev;
		}
	}

	return(new_ss);
}

struct symstr *
symstr_prev(struct symstr *ss, struct alphabet *alpha, int *ok)
{
	struct symstr *new_ss;
	struct symstr_component *sc;
	struct symbol *sym;

	new_ss = symstr_dup(ss);

	sc = new_ss->tail;
	sym = symbol_prev(sc->sym, alpha);
	if (sym != NULL) {
		sc->sym = sym;
		*ok = 1;
	} else {
		*ok = 0;
	}

	return(new_ss);
}

struct symstr *
symstr_next(struct symstr *ss, struct alphabet *alpha, int *ok)
{
	struct symstr *new_ss;
	struct symstr_component *sc;
	struct symbol *sym;

	new_ss = symstr_dup(ss);

	sc = new_ss->tail;
	sym = symbol_next(sc->sym, alpha);
	if (sym != NULL) {
		sc->sym = sym;
		*ok = 1;
	} else {
		*ok = 0;
	}

	return(new_ss);
}

void
symstr_dump(struct symstr *ss)
{
	fprintf(stderr, "{");
	if (ss == NULL) {
		fprintf(stderr, "}");
		return;
	}
	symstr_components_dump(ss->head);
	fprintf(stderr, "}");
}

void
symstr_components_dump(struct symstr_component *sc)
{
	while (sc != NULL) {
		symstr_component_dump(sc);
		if (sc->next != NULL)
			fprintf(stderr, " ");
		sc = sc->next;
	}
}

void
symstr_component_dump(struct symstr_component *sc)
{
	if (sc->alphasym == NULL) {
		if (sc->op == SYMSTR_OP_NOP) {
			symbol_print(sc->sym);
		} else {
			fprintf(stderr, "(");
			if (sc->op == SYMSTR_OP_PRED)
				fprintf(stderr, "pred ");
			else if (sc->op == SYMSTR_OP_SUCC)
				fprintf(stderr, "succ ");
			else if (sc->op == SYMSTR_OP_PREV)
				fprintf(stderr, "prev ");
			else if (sc->op == SYMSTR_OP_NEXT)
				fprintf(stderr, "next ");
			symbol_print(sc->sym);
			if (sc->fmode != NULL) {
				fprintf(stderr, " | ");
				symstr_components_dump(sc->fmode->head);
			}
			fprintf(stderr, ")");
		}
	} else {
		fprintf(stderr, "(");
		symbol_print(sc->sym);
		fprintf(stderr, " = ");
		symbol_print(sc->alphasym);
		if (sc->many)
			fprintf(stderr, "+");
		fprintf(stderr, ")");
	}
}

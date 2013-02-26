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
 * parse.c
 * Recursive-descent parser for 2Iota.
 * $Id: parse.c 518 2010-04-28 17:48:38Z cpressey $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scan.h"
#include "parse.h"
#include "symbol.h"
#include "symstr.h"
#include "alphabet.h"
#include "event.h"
#include "etime.h"
#include "2iota.h"

static struct caused_by_table *caused_by;

/* ---------------- EVENT STRUCTURE (beta-Juliet) ----------------*/

void
two_iota(struct scan_st *sc)
{
	caused_by = caused_by_table_new();

	decl(sc);
	while (tokeq(sc, ";")) {
		scan(sc);
		decl(sc);
	}
	scan_expect(sc, ".");

	caused_by_reconcile(event_table, caused_by);
}

void
decl(struct scan_st *sc)
{
	if (tokeq(sc, "alphabet")) {
		struct symbol *sym;
		struct alphabet *alpha;

		alpha = alphabet_new();
		scan(sc);
		sym = symbol_name(sc, astab, SYM_TYPE_ALPHABET, SYM_LOOKUP_UNIQUE);
		sym->alpha = alpha;
		while(tokeq(sc, ",")) {
			scan(sc);
			sym = symbol_name(sc, gstab, SYM_TYPE_SYMBOL, SYM_LOOKUP_DEFINE);
			alphabet_append(alpha, sym);
		}
	} else if (tokeq(sc, "event")) {
		struct event *e;

		e = event_new(event_table);	/* XXX global? */
		scan(sc);
		event_decl_name(sc, e);
		while(tokeq(sc, ",")) {
			scan(sc);
			property(sc, e);
		}
	} else
		scan_error(sc, "Expected 'alphabet' or 'event'");
}	

void
property(struct scan_st *sc, struct event *e)
{
	if (tokeq(sc, "causes")) {
		struct symstr *ss;
		struct etime et = etime_zero;

		scan(sc);
		ss = event_appl_name(sc, e);
		if (tokeq(sc, "after")) {
			scan(sc);
			time_spec(sc, &et);
			(void)event_consequence_append(e, ss, &et);
		} else {
			(void)event_consequence_append(e, ss, NULL);
		}
		while(tokeq(sc, "when")) {
			scan(sc);
			when_term(sc, e);
		}
	} else if (tokeq(sc, "caused")) {
		struct symstr *ss;

		scan(sc);
		if (tokeq(sc, "by") || tokeq(sc, "after") || tokeq(sc, "before")) {
			scan(sc);
			ss = event_appl_name(sc, e);
                        if (!symstr_is_literal(ss)) {
                              scan_error(sc, "Expected literal event name");
                        }
                        if (!symstr_is_literal(e->name)) {
                              scan_error(sc, "'caused' clause may only appear "
                                             "on events with literal names");
                        }
			(void)caused_by_add(caused_by, ss, e);
		} else {
			scan_error(sc, "Expected 'by', 'after', or 'before'");
		}
	} else if (tokeq(sc, "duration")) {
		scan(sc);
		time_spec(sc, &e->duration);
	} else
		scan_error(sc, "Expected 'causes', 'caused', or 'duration'");
}

void
when_term(struct scan_st *sc, struct event *e)
{
	struct symstr *earlier, *later;

	later = event_appl_name(sc, e);
	scan_expect(sc, ">");
	earlier = event_appl_name(sc, e);
	consequence_condition_append(e->head, earlier, later);
}

void
time_spec(struct scan_st *sc, struct etime *et)
{
	double n, f;
	long secs, msecs;
	char unit[16];

	while (isdigit((int)sc->token[0])) {
		n = atof(sc->token);
		scan(sc);
		strncpy(unit, sc->token, 16);
		scan(sc);

		if (!strcmp(unit, "d")) {
			f = (3600.0 * 24.0);
		} else if (!strcmp(unit, "h")) {
			f = 3600.0;
		} else if (!strcmp(unit, "m")) {
			f = 60.0;
		} else if (!strcmp(unit, "s")) {
			f = 1.0;
		} else if (!strcmp(unit, "ms")) {
			f = 0.001;
		} else {
			scan_error(sc, "Expected 'd', 'h', 'm', 's', or 'ms'");
			break;
		}
		
		secs = f * n;
		msecs = ((f * n) - secs) * 1000;
		etime_add(et, 0, 0, 0, secs, msecs, 0);
	}
}

/* -------------------- EVENT NAMES (Portia) --------------------*/

void
event_decl_name(struct scan_st *sc, struct event *e)
{
	struct symbol *sym;

	while (tokne(sc, ",") && tokne(sc, ";") && tokne(sc, ".")) {
		if (tokeq(sc, "(")) {
			scan(sc);
			sym = symbol_name(sc, e->params, SYM_TYPE_PARAMETER, SYM_LOOKUP_UNIQUE);
			scan_expect(sc, "=");
			match_expr(sc, e, sym);
			scan_expect(sc, ")");
		} else {
			sym = symbol_name(sc, gstab, SYM_TYPE_LITERAL, SYM_LOOKUP_DEFINE);
			symstr_append(e->name, sym, NULL, 0, SYMSTR_OP_NOP);
		}
	}
}

void
match_expr(struct scan_st *sc, struct event *e, struct symbol *param_sym)
{
	int many = 0;
	struct symbol *alpha_sym;

	alpha_sym = symbol_name(sc, astab, SYM_TYPE_ALPHABET, SYM_LOOKUP_EXTANT);
	if (tokeq(sc, "+")) {
		scan(sc);
		many = 1;
	}
	symstr_append(e->name, param_sym, alpha_sym, many, SYMSTR_OP_NOP);
	/*
	 * Also set the alphabet on the parameter, so that we
	 * know what to do when we later succ() or pred() it.
	 */
	param_sym->alpha = alpha_sym->alpha;
}

struct symstr *
event_appl_name(struct scan_st *sc, struct event *e)
{
	struct symstr *ss;
	struct symbol *sym;

	ss = symstr_new();
	while (tokne(sc, ",") && tokne(sc, ";") && tokne(sc, ".") &&
	       tokne(sc, ">") && tokne(sc, "after") && tokne(sc, "when")) {
		if (tokeq(sc, "(")) {
			scan(sc);
			alphabet_expr(sc, e, ss);
			scan_expect(sc, ")");
		} else {
			sym = symbol_name(sc, gstab,
			    SYM_TYPE_LITERAL, SYM_LOOKUP_DEFINE);
			symstr_append(ss, sym, NULL, 0, SYMSTR_OP_NOP);
		}
	}
	return(ss);
}

void
alphabet_expr(struct scan_st *sc, struct event *e, struct symstr *ss)
{
	struct symstr *fm_ss;
	struct symstr_component *psymc, *nsymc;

	psymc = alphabet_term(sc, e, ss);
	while (tokeq(sc, "|")) {
		scan(sc);
		fm_ss = symstr_new();
		nsymc = alphabet_term(sc, e, fm_ss);
		psymc->fmode = fm_ss;
		psymc = nsymc;
	}
}

struct symstr_component *
alphabet_term(struct scan_st *sc, struct event *e, struct symstr *ss)
{
	struct symbol *sym;
	struct symstr_component *symc = NULL;

	if (tokeq(sc, "succ")) {
		scan(sc);
		sym = symbol_name(sc, e->params, SYM_TYPE_PARAMETER, SYM_LOOKUP_EXTANT);
		symc = symstr_append(ss, sym, NULL, 0, SYMSTR_OP_SUCC);
	} else if (tokeq(sc, "pred")) {
		scan(sc);
		sym = symbol_name(sc, e->params, SYM_TYPE_PARAMETER, SYM_LOOKUP_EXTANT);
		symc = symstr_append(ss, sym, NULL, 0, SYMSTR_OP_PRED);
	} else if (tokeq(sc, "prev")) {
		scan(sc);
		sym = symbol_name(sc, e->params, SYM_TYPE_PARAMETER, SYM_LOOKUP_EXTANT);
		symc = symstr_append(ss, sym, NULL, 0, SYMSTR_OP_PREV);
	} else if (tokeq(sc, "next")) {
		scan(sc);
		sym = symbol_name(sc, e->params, SYM_TYPE_PARAMETER, SYM_LOOKUP_EXTANT);
		symc = symstr_append(ss, sym, NULL, 0, SYMSTR_OP_NEXT);
	} else if (tokeq(sc, "first")) {
		scan(sc);
		sym = symbol_name(sc, astab, SYM_TYPE_ALPHABET, SYM_LOOKUP_EXTANT);
		sym = alphabet_first(sym->alpha);
		symc = symstr_append(ss, sym, NULL, 0, SYMSTR_OP_NOP);
	} else if (tokeq(sc, "last")) {
		scan(sc);
		sym = symbol_name(sc, astab, SYM_TYPE_ALPHABET, SYM_LOOKUP_EXTANT);
		sym = alphabet_last(sym->alpha);
		symc = symstr_append(ss, sym, NULL, 0, SYMSTR_OP_NOP);
	} else {
		sym = symbol_name(sc, gstab, SYM_TYPE_LITERAL, SYM_LOOKUP_DEFINE);
		symc = symstr_append(ss, sym, NULL, 0, SYMSTR_OP_NOP);
	}

	return(symc);
}

/* ------------ BASIC NAMES ------------- */

struct symbol *
symbol_name(struct scan_st *sc, struct symbol_table *stab, int type, int lookup)
{
	char name[128];
	struct symbol * sym;

	strncpy(name, sc->token, 128);
	scan(sc);

	sym = symbol_lookup(stab, name);

	if (lookup == SYM_LOOKUP_UNIQUE) {
		if (sym != NULL) {
			scan_error(sc, "Symbol '%s' already defined", name);
			return(NULL);
		} else {
			sym = symbol_define(stab, name, type);
			return(sym);
		}
	} else if (lookup == SYM_LOOKUP_DEFINE) {
		if (sym != NULL) {
			return(sym);
		} else {
			sym = symbol_define(stab, name, type);
			return(sym);
		}
	} else if (lookup == SYM_LOOKUP_EXTANT) {
		if (sym == NULL) {
			scan_error(sc, "Symbol '%s' not defined", name);
			return(NULL);
		} else {
			return(sym);
		}
	}

	/* internal error */
	return(NULL);
}

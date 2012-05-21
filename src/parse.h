/*
 * parse.h
 * Parser structures and prototypes for 2Iota.
 * $Id: parse.h 518 2010-04-28 17:48:38Z cpressey $
 */

#ifndef __PARSE_H
#define __PARSE_H

#include "scan.h"
#include "symbol.h"
#include "symstr.h"
#include "event.h"
#include "etime.h"

void		 two_iota(struct scan_st *);
void		 decl(struct scan_st *);
void		 property(struct scan_st *, struct event *);
void		 when_term(struct scan_st *, struct event *);
void		 time_spec(struct scan_st *, struct etime *);

void		 event_decl_name(struct scan_st *, struct event *);
void		 match_expr(struct scan_st *, struct event *, struct symbol *);
struct symstr	*event_appl_name(struct scan_st *, struct event *);
void		 alphabet_expr(struct scan_st *, struct event *, struct symstr *);
struct symstr_component
		*alphabet_term(struct scan_st *, struct event *, struct symstr *);

struct symbol	*symbol_name(struct scan_st *, struct symbol_table *, int, int);

#endif /* !__PARSE_H */

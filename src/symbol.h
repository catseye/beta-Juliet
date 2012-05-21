/*
 * symbol.h
 * Symbol structures and prototypes for 2Iota.
 * $Id: symbol.h 518 2010-04-28 17:48:38Z cpressey $
 */

#ifndef __SYMBOL_H
#define __SYMBOL_H

/*
 * Along with the global symbol table, individual symbol
 * tables exist for each event (for local parameters.)
 */
struct symbol_table {
	struct symbol	*head;
};

struct symbol {
	char		*token;		/* lexeme making up the symbol */
	int		 type;		/* SYM_TYPE_*, below */
	struct symbol	*next;		/* next symbol in symbol table */
	/*
	 * For SYM_TYPE_ALPHABET and SYM_TYPE_PARAMETER,
	 * 'alpha' points to the associated struct alphabet.
	 */
	struct alphabet	*alpha;
	/*
	 * For SYM_TYPE_PARAMETER symbols in an
	 * event-local symbol table, 'data' points to a symstr
	 * representing each parameter's bound value.
	 */
	struct symstr	*data;
};

#define SYM_TYPE_LITERAL	0	/* symbols which 'just appear' (part of event name) */
#define SYM_TYPE_ALPHABET	1	/* symbol is the name of an alphabet */
#define SYM_TYPE_SYMBOL		2	/* symbol is a symbol in an alphabet */
#define SYM_TYPE_PARAMETER	3	/* symbol is the name of a parameter */

#define SYM_LOOKUP_UNIQUE	0	/* symbol must not already exist */
#define SYM_LOOKUP_DEFINE	1	/* symbol may or may not already exist */
#define SYM_LOOKUP_EXTANT	2	/* symbol must already exist */

struct symbol_table	*symbol_table_new(void);
void			 symbol_table_free(struct symbol_table *);
void			 symbol_table_unbind(struct symbol_table *);
struct symbol		*symbol_define(struct symbol_table *, char *, int);
struct symbol		*symbol_lookup(struct symbol_table *, char *);
void			 symbol_free(struct symbol *);
struct symbol		*symbol_prev(struct symbol *, struct alphabet *);
struct symbol		*symbol_next(struct symbol *, struct alphabet *);

void			 symbol_table_dump(int, struct symbol_table *);
void			 symbol_print(struct symbol *);
void			 symbol_dump(int, struct symbol *);

#endif /* !__SYMBOL_H */

/*
 * alphabet.h
 * Alphabet structures and prototypes for 2Iota.
 * $Id: alphabet.h 518 2010-04-28 17:48:38Z cpressey $
 */

#ifndef __ALPHABET_H
#define __ALPHABET_H

#include "symbol.h"
#include "symstr.h"

struct alphabet {
	struct alpha_entry	*head;
	struct alpha_entry	*tail;
};

struct alpha_entry {
	struct symbol		*sym;
	struct alpha_entry	*next;
	struct alpha_entry	*prev;
};

struct alphabet		*alphabet_new(void);
void			 alphabet_append(struct alphabet *, struct symbol *);
void			 alphabet_free(struct alphabet *);
struct alpha_entry	*alphabet_lookup(struct alphabet *, struct symbol *);
struct symbol		*alphabet_first(struct alphabet *);
struct symbol		*alphabet_last(struct alphabet *);

#endif /* !__ALPHABET_H */

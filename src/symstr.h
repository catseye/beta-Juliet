/*
 * symstr.h
 * Symbol string structures and prototypes for 2Iota.
 * $Id: symstr.h 54 2004-04-23 22:51:09Z catseye $
 */

#ifndef __SYMSTR_H
#define __SYMSTR_H

#include "symbol.h"

struct symstr {
	struct symstr_component	*head;
	struct symstr_component	*tail;
};

struct symstr_component {
	struct symstr_component	*next;
	struct symstr_component	*prev;
	struct symbol		*sym;		/* may be lit or param */
	struct symbol		*alphasym;	/* indicates a binding:
						   alphasym MUST be
						   SYM_TYPE_ALPHABET */
	int			 many;		/* indicates '+' rep OK */
	int			 op;		/* for param in subst, the op */
	struct symstr		*fmode;		/* failure mode alternative */
};

#define	SYMSTR_OP_NOP			 0	/* No operation */
#define	SYMSTR_OP_SUCC			 1	/* Successor */
#define	SYMSTR_OP_PRED			 2	/* Predecessor */
#define	SYMSTR_OP_NEXT			 3	/* Next in alphabet */
#define	SYMSTR_OP_PREV			 4	/* Previous in alphabet */

#define	SYMSTR_BIND_OK			 0
#define	SYMSTR_BIND_LITERAL_MISMATCH	 1
#define	SYMSTR_BIND_STRING_TOO_SHORT	 2
#define	SYMSTR_BIND_PATTERN_TOO_SHORT	 3
#define	SYMSTR_BIND_BAD_STRING		 4
#define	SYMSTR_BIND_BAD_PATTERN		 5
#define	SYMSTR_BIND_OUTSIDE_ALPHABET	 6
#define	SYMSTR_BIND_NOT_IMPLEMENTED	 7

extern char *		 symstr_bind_result[];

/* housekeeping */
struct symstr		*symstr_new(void);
struct symstr		*symstr_create(struct symbol_table *, char *);
struct symstr		*symstr_dup(struct symstr *);
void			 symstr_free(struct symstr *);
struct symstr_component	*symstr_component_new(struct symbol *, struct symbol *,
					      int, int);
void			 symstr_component_free(struct symstr_component *);

/* building */
struct symstr_component	*symstr_append(struct symstr *, struct symbol *,
					struct symbol *, int, int);
struct symstr_component	*symstr_prepend(struct symstr *, struct symbol *,
					struct symbol *, int, int);
void			 symstr_append_symstr(struct symstr *,
					      struct symstr *);
void			 symstr_remove(struct symstr *, struct symstr_component *);

/* specificity */
void			 symstr_specificity(struct symstr *, int *, int *);

/* substitution & binding */
struct symstr		*symstr_substitute(struct symstr *,
					   struct symbol_table *);
void			 symstr_component_substitute(struct symstr_component *,
						     struct symstr *,
						     struct symstr *);
int			 symstr_bind(struct symstr *, struct symstr *,
					struct symbol_table *);

/* alphabetical / Peano arithmetic */
struct symstr		*symstr_succ(struct symstr *, struct alphabet *, int *);
struct symstr		*symstr_pred(struct symstr *, struct alphabet *, int *);
struct symstr		*symstr_prev(struct symstr *, struct alphabet *, int *);
struct symstr		*symstr_next(struct symstr *, struct alphabet *, int *);

/* dumping and debugging */
void			 symstr_dump(struct symstr *);
void			 symstr_components_dump(struct symstr_component *);
void			 symstr_component_dump(struct symstr_component *);

#endif /* !__SYMSTR_H */

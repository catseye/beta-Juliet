/*
 * ehist.h
 * Event history structures and prototypes for 2Iota.
 * $Id: ehist.h 518 2010-04-28 17:48:38Z cpressey $
 */

#ifndef __EHIST_H
#define __EHIST_H

#include "etime.h"

struct ehist {
	struct ehist_entry	*head;
};

struct ehist_entry {
	struct ehist_entry	*next;
	struct symstr		*name;		/* name of event that happened */
	struct etime		when;		/* when it happened */
};

struct ehist		*ehist_new(void);
struct ehist_entry	*ehist_find(struct ehist *, struct symstr *);
struct ehist_entry	*ehist_update(struct ehist *, struct symstr *,
				      struct etime *);
int			 ehist_was_later(struct ehist *,
					 struct symstr *, struct symstr *);
void			 ehist_dump(struct ehist *);

#endif /* !__EHIST_H */

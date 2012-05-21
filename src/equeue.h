/*
 * equeue.h
 * Event queue structures and prototypes for 2Iota.
 * $Id: equeue.h 518 2010-04-28 17:48:38Z cpressey $
 */

#ifndef __EQUEUE_H
#define __EQUEUE_H

#include "etime.h"

struct equeue {
	struct equeue_entry	*head;
};

struct equeue_entry {
	struct equeue_entry	*next;
	struct equeue_entry	*prev;
	struct symstr		*name;		/* name of event to trigger */
	struct etime		delay;
};

struct equeue		*equeue_new(void);
void			 equeue_free(struct equeue *);
struct equeue_entry	*equeue_insert(struct equeue *, struct symstr *,
					struct etime *);
struct equeue_entry	*equeue_remove(struct equeue *);
void			 equeue_entry_free(struct equeue_entry *);
void			 equeue_transfer(struct equeue *, struct equeue *);
void			 equeue_time_passes(struct equeue *, struct etime *);

void			 equeue_dump(struct equeue *);

#endif /* !__EQUEUE_H */

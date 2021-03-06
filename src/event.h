/*
 * event.h
 * Event structures and prototypes for 2Iota.
 * $Id: event.h 518 2010-04-28 17:48:38Z cpressey $
 */

#ifndef __EVENT_H
#define __EVENT_H

#include "equeue.h"
#include "ehist.h"
#include "etime.h"

struct event_table {
	struct event		*head;
};

struct event {
	struct event		*next;
	struct symstr		*name;		/* name of event */
	struct consequence	*head;		/* what happens on trigger */
	struct symbol_table	*params;	/* local bindings */
	struct etime		duration;	/* for v1.0 compat */
};

struct consequence {
	struct consequence	*next;
	struct symstr		*effect;	/* name of event to trigger */
	struct etime		delay;		/* trigger event after this */
	struct condition	*head;		/* trigger conditions 'when' */
};

struct condition {
	struct condition	*next;
	struct symstr		*earlier;
	struct symstr		*later;
};

struct caused_by_table {
	struct caused_by        *head;
};

struct caused_by {
	struct caused_by	*next;
	struct symstr		*cause; 	/* name of triggering event */
	struct event		*effect;	/* event to trigger */
};


struct event_table	*event_table_new(void);
struct event		*event_new(struct event_table *);
struct consequence	*event_consequence_append(struct event *,
						  struct symstr *,
						  struct etime *);
void			 event_dump(struct event *);
void			 event_table_dump(struct event_table *);
struct condition	*consequence_condition_append(struct consequence *,
						      struct symstr *,
						      struct symstr *);
struct event		*event_find(struct event_table *, struct symstr *);
int			 event_happen(struct event *, struct equeue *,
				      struct ehist *);

struct caused_by_table	*caused_by_table_new(void);
struct caused_by	*caused_by_add(struct caused_by_table *,
				       struct symstr *,
				       struct event *);
void			 caused_by_reconcile(struct event_table *,
					     struct caused_by_table *);

#endif /* !__EVENT_H */

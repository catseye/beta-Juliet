/*
 * engine.h
 * Event processing engine structures and prototypes for 2Iota.
 * $Id: engine.h 54 2004-04-23 22:51:09Z catseye $
 */

#ifndef __ENGINE_H
#define __ENGINE_H

#include <signal.h>

extern volatile sig_atomic_t caught_signal;

int	ii_engine_run(struct event_table *, struct equeue *, struct ehist *);
int	ii_engine_walk(struct event_table *, struct equeue *, struct ehist *);
int	ii_engine_step(struct event_table *, struct equeue *, struct ehist *,
			struct equeue *);

#endif /* !__ENGINE_H */

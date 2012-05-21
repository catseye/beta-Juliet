/*
 * etime.h
 * Time/duration structures and prototypes for 2Iota.
 * $Id: etime.h 54 2004-04-23 22:51:09Z catseye $
 */

#ifndef __ETIME_H
#define __ETIME_H

#include <sys/time.h>
#include <time.h>

struct etime {
	struct timespec		tv;
};

void			 etime_set(struct etime *,
				   long, long, long, long, long, long);
void			 etime_add(struct etime *,
				   long, long, long, long, long, long);
void			 etime_now(struct etime *);
int			 etime_compare(struct etime *, struct etime *);
void			 etime_delta(struct etime *, struct etime *, int);
int			 etime_sleep(struct etime *);

void			 etime_dump(struct etime *);

extern struct etime	 etime_zero;

#endif /* !__ETIME_H */

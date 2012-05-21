/*
 * etime.h
 * Time/duration structures and prototypes for 2Iota.
 * $Id: etime.h 545 2010-04-30 14:27:14Z cpressey $
 */

#ifndef __ETIME_H
#define __ETIME_H

#include <time.h>

struct etime {
        time_t  sec;
        long    nsec;
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

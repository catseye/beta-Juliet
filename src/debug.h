/*
 * debug.h
 * Debugging structures and prototypes for 2Iota.
 * $Id: debug.h 518 2010-04-28 17:48:38Z cpressey $
 */

void		debug_trace(int, const char *, ...);

void		debug_dump(const char *);
void		debug_dump_all(void);

extern int	trace_flags;

#define		TRACE_EVENTS	0x01
#define		TRACE_QUEUE	0x02
#define		TRACE_HISTORY	0x04

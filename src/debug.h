/*
 * debug.h
 * Debugging structures and prototypes for 2Iota.
 * $Id: debug.h 54 2004-04-23 22:51:09Z catseye $
 */

void		debug_trace(int, char *, ...);

void		debug_dump(char *);
void		debug_dump_all(void);

extern int	trace_flags;

#define		TRACE_EVENTS	0x01
#define		TRACE_QUEUE	0x02
#define		TRACE_HISTORY	0x04

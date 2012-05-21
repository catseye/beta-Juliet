/*
 * 2iota.h
 * Global variables and macros for 2Iota.
 * $Id: 2iota.h 54 2004-04-23 22:51:09Z catseye $
 */

#ifndef __2IOTA_H
#define __2IOTA_H

#ifdef DETECT_LEAKS
#include <leak_detector.h>
#endif

#include <stdio.h>
#include <err.h>

#ifdef TRACK_ALLOCATION
#define TRACK_MALLOC(pointer, structdesc)				\
		fprintf(stderr, "***** MALLOC'ed " structdesc		\
		    " %08lx\n", pointer);
#define TRACK_FREE(pointer, structdesc)					\
		fprintf(stderr, "***** FREE'ed " structdesc		\
		    " %08lx\n", pointer);
#else
#define	TRACK_MALLOC(pointer, structdesc)
#define	TRACK_FREE(pointer, structdesc)
#endif

#define MALLOC(pointer, structure, structdesc)				\
	{								\
		if ((pointer = (struct structure *)			\
		    malloc(sizeof(struct structure))) == NULL)		\
			errx(1, "Can't allocate " structdesc);		\
		TRACK_MALLOC(pointer, structdesc);			\
	}

#define FREE(pointer, structdesc)					\
	{								\
		free(pointer);						\
		TRACK_FREE(pointer, structdesc);			\
	}

extern struct symbol_table	*gstab;	/* general symbol table */
extern struct symbol_table	*astab;	/* symbol table for alphabets */
extern struct event_table	*etab;	/* global table of events */
extern struct equeue		*eq;	/* global event queue */
extern struct ehist		*eh;	/* global event history log */

extern int			do_dump;
extern int			interactive;

void				usage(void);

#endif /* !__2IOTA_H */

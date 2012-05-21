/*
 * scan.h
 * Lexical scanner structures and prototypes for 2Iota.
 * $Id: scan.h 54 2004-04-23 22:51:09Z catseye $
 */

#ifndef __SCAN_H
#define __SCAN_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct scan_st {
	FILE	*in;
	char	*token;
	int	 lino;
	int	 columno;
};

#define tokeq(sc, x)	(!strcmp(sc->token, x))
#define tokne(sc, x)	(strcmp(sc->token, x))

extern struct scan_st *scan_open(char *);
extern void scan_close(struct scan_st *);
extern void scan_error(struct scan_st *, char *, ...);
extern void scan(struct scan_st *);
extern void scan_expect(struct scan_st *, char *);

#endif /* !__SCAN_H */

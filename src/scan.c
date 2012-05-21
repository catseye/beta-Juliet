/*
 * Copyright (c)2004 Cat's Eye Technologies.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * 
 *   Neither the name of Cat's Eye Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
/*
 * scan.c
 * Lexical scanner for 2Iota.
 * $Id: scan.c 54 2004-04-23 22:51:09Z catseye $
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "scan.h"
#include "2iota.h"

struct scan_st *
scan_open(char *filename)
{
	struct scan_st *sc;

	MALLOC(sc, scan_st, "scanner");

	if ((sc->token = (char *)malloc(256 * sizeof(char))) == NULL) {
		free(sc);
		return(NULL);
	}
	if ((sc->in = fopen(filename, "r")) == NULL) {
		free(sc->token);
		free(sc);
		return(NULL);
	}
	sc->lino = 1;
	sc->columno = 1;
	scan(sc);		/* prime the pump */
	return(sc);
}

void
scan_close(struct scan_st *sc)
{	
	fclose(sc->in);
	free(sc->token);
	free(sc);
}

void
scan_error(struct scan_st *sc, char *fmt, ...)
{
	va_list args;
	char err[256];

	va_start(args, fmt);
	vsnprintf(err, 255, fmt, args);

	fprintf(stderr, "Error (line %d, column %d, token '%s'): %s.\n",
	    sc->lino, sc->columno, sc->token, err);
}

int
scan_char(struct scan_st *sc, char *x)
{	
	*x = (char)getc(sc->in); sc->columno++;
	if (feof(sc->in)) {
		sc->token[0] = 0;
		return(0);
	}
	return(1);
}

void
scan(struct scan_st *sc)
{
	char x;
	int i = 0;

	sc->token[0] = 0;
	if (feof(sc->in)) return;
	if (!scan_char(sc, &x)) return;

	/* Skip whitespace. */

top:
	while (isspace(x)) {
		if (x == '\n') {
			sc->lino++;
			sc->columno = 0;
		}
		if (!scan_char(sc, &x)) return;
	}

	/* Skip comments. */

	if (x == '/') {
		while (x != '\n') {
			if (!scan_char(sc, &x)) return;
		}
		goto top;
	}

	/*
	 * Scan decimal numbers.  Must start with a
	 * digit (not a sign or decimal point.)
	 */

	if (isdigit(x) && !feof(sc->in)) {
		while ((isdigit(x) || x == '.') && !feof(sc->in)) {
			sc->token[i++] = x;
			if (!scan_char(sc, &x)) return;
		}
		ungetc(x, sc->in);
		sc->columno--;
		sc->token[i] = 0;
		return;
	}

	/* Scan alphanumeric tokens. */

	if (isalpha(x) && !feof(sc->in)) {
		while ((isalpha(x) || isdigit(x) || x == '_') && !feof(sc->in)) {
			sc->token[i++] = x;
			if (!scan_char(sc, &x)) return;
		}
		ungetc(x, sc->in);
		sc->columno--;
		sc->token[i] = 0;
		return;
	} else {
		sc->token[0] = x;
		sc->token[1] = 0;
		return;
	}
}

void
scan_expect(struct scan_st *sc, char *x)
{
	if (!strcmp(sc->token, x)) {
		scan(sc);
	} else {
		scan_error(sc, "Expected '%s'", x);
	}
}

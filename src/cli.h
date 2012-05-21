/*
 * cli.h
 * Command-line interface structures and prototypes for 2Iota.
 * $Id: cli.h 518 2010-04-28 17:48:38Z cpressey $
 */

void		cmdline(void);
int		do_cmd(char *);
int		parse_cmd(char *, char **, char **);
void		help(void);

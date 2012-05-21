/*
 * cli.h
 * Command-line interface structures and prototypes for 2Iota.
 * $Id: cli.h 54 2004-04-23 22:51:09Z catseye $
 */

void		cmdline(void);
int		do_cmd(char *);
int		parse_cmd(char *, char **, char **);
void		help(void);

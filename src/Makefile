# GNU Makefile for 2Iota.
# $Id: Makefile 658 2010-07-21 17:33:26Z cpressey $

PROG=2iota
CC?=gcc
STRIP?=strip
O?=.o
EXE?=

WARNS?=	-Werror -W -Wall -Wstrict-prototypes -Wmissing-prototypes \
	-Wpointer-arith	-Wno-uninitialized -Wreturn-type -Wcast-qual \
	-Wwrite-strings -Wswitch -Wshadow -Wcast-align -Wchar-subscripts \
	-Winline -Wnested-externs -Wredundant-decls

ifeq ($(CC),8cc)
  WARNS=-Wall -Werror
else
  ifeq ($(CC),pcc)
    CFLAGS+=-I/usr/include/linux -I/usr/lib/gcc/x86_64-linux-gnu/4.6/include/
  else
    ifdef ANSI
      CFLAGS+= -ansi -pedantic
    else
      CFLAGS+= -std=c99 -D_POSIX_C_SOURCE=200112L
    endif
  endif
endif

CFLAGS+= ${WARNS} ${EXTRA_CFLAGS}

ifdef DEBUG
  CFLAGS+= -g
endif

ifdef DETECT_LEAKS
  CFLAGS+= -DDETECT_LEAKS -I/usr/local/include -L/usr/local/lib
  LIBS+= -lgc
endif

OBJS=	symbol${O} symstr${O} \
	alphabet${O} event${O} \
	etime${O} equeue${O} ehist${O} \
	scan${O} parse${O} \
	engine${O} cli${O} \
	debug${O} main${O}

all: ${PROG}${EXE}

${PROG}${EXE}: $(OBJS)
	$(CC) $(OBJS) -o ${PROG} $(LIBS)

symbol${O}: symbol.c symbol.h
	$(CC) $(CFLAGS) -c symbol.c -o symbol${O}

symstr${O}: symstr.c symstr.h symbol.h alphabet.h
	$(CC) $(CFLAGS) -c symstr.c -o symstr${O}

alphabet${O}: alphabet.c alphabet.h symbol.h
	$(CC) $(CFLAGS) -c alphabet.c -o alphabet${O}

event${O}: event.c event.h alphabet.h symbol.h symstr.h debug.h
	$(CC) $(CFLAGS) -c event.c -o event${O}

etime${O}: etime.c etime.h
	$(CC) $(CFLAGS) -c etime.c -o etime${O}

equeue${O}: equeue.c equeue.h symstr.h debug.h
	$(CC) $(CFLAGS) -c equeue.c -o equeue${O}

ehist${O}: ehist.c ehist.h symstr.h debug.h
	$(CC) $(CFLAGS) -c ehist.c -o ehist${O}

scan${O}: scan.c scan.h
	$(CC) $(CFLAGS) -c scan.c -o scan${O}

parse${O}: parse.c parse.h scan.h symbol.h
	$(CC) $(CFLAGS) -c parse.c -o parse${O}

engine${O}: engine.c engine.h event.h equeue.h
	$(CC) $(CFLAGS) -c engine.c -o engine${O}

cli${O}: cli.c cli.h symstr.h event.h debug.h
	$(CC) $(CFLAGS) -c cli.c -o cli${O}

debug${O}: debug.c debug.h
	$(CC) $(CFLAGS) -c debug.c -o debug${O}

main${O}: main.c cli.h debug.h
	$(CC) $(CFLAGS) -c main.c -o main${O}

clean:
	rm -f *.o *.core *.exe ${PROG}${EXE}

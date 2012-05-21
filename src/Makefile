# BSD Makefile for 2Iota.
# $Id: Makefile 54 2004-04-23 22:51:09Z catseye $

CC=	gcc
CFLAGS=	-I/usr/local/include -L/usr/local/lib -Wall
LIBS=	

.ifdef DEBUG
CFLAGS+= -g
.endif

.ifdef DETECT_LEAKS
CFLAGS+= -DDETECT_LEAKS -I/usr/local/include -L/usr/local/lib
LIBS+=	 -lgc
.endif

OBJS=	symbol.o symstr.o \
	alphabet.o event.o \
	etime.o equeue.o ehist.o \
	scan.o parse.o \
	engine.o cli.o \
	debug.o main.o

all: 2iota

2iota: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o 2iota $(LIBS)

symbol.o: symbol.c symbol.h
	$(CC) $(CFLAGS) -c symbol.c -o symbol.o

symstr.o: symstr.c symstr.h symbol.h alphabet.h
	$(CC) $(CFLAGS) -c symstr.c -o symstr.o

alphabet.o: alphabet.c alphabet.h symbol.h
	$(CC) $(CFLAGS) -c alphabet.c -o alphabet.o

event.o: event.c event.h alphabet.h symbol.h symstr.h debug.h
	$(CC) $(CFLAGS) -c event.c -o event.o

etime.o: etime.c etime.h
	$(CC) $(CFLAGS) -c etime.c -o etime.o

equeue.o: equeue.c equeue.h symstr.h debug.h
	$(CC) $(CFLAGS) -c equeue.c -o equeue.o

ehist.o: ehist.c ehist.h symstr.h debug.h
	$(CC) $(CFLAGS) -c ehist.c -o ehist.o

scan.o: scan.c scan.h
	$(CC) $(CFLAGS) -c scan.c -o scan.o

parse.o: parse.c parse.h scan.h symbol.h
	$(CC) $(CFLAGS) -c parse.c -o parse.o

engine.o: engine.c engine.h event.h equeue.h
	$(CC) $(CFLAGS) -c engine.c -o engine.o

cli.o: cli.c cli.h symstr.h event.h debug.h
	$(CC) $(CFLAGS) -c cli.c -o cli.o

debug.o: debug.c debug.h
	$(CC) $(CFLAGS) -c debug.c -o debug.o

main.o: main.c cli.h debug.h
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f *.o *.core 2iota

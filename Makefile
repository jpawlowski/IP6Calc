##############################
# Makefile for ip6calc

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=

CFILES=$(shell echo *.c)
OFILES=$(CFILES:.c=.o)

ip6calc: $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

help.o: help.h

help.h: help.txt genhelp.sh
	./genhelp.sh

clean:
	-rm -f *.o *~ *.bak help.h

distclean: clean
	-rm -f ip6calc

.PHONY: clean distclean

config configure:
	@echo Dummy configure.

install:
	echo $(DISTROOT)

##############################
# Makefile for ip6calc

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=
INSTALL=install

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
	-rm -f ip6calc .config.make

.PHONY: clean distclean

.config.make:
	./configure

include .config.make

install:
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -t $(DESTDIR)$(BINDIR) -s ip6calc
	$(INSTALL) -t $(DESTDIR)$(BINDIR) ip4toip6
	$(INSTALL) -d $(DESTDIR)$(DOCDIR)
	$(INSTALL) -m 644 -t $(DESTDIR)$(DOCDIR) help.txt
	$(INSTALL) -m 644 -t $(DESTDIR)$(DOCDIR) README

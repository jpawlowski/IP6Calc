##############################
# Makefile for ip6calc

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=

ip6calc: ip6calc.o calculate.o help.o output.o parseaddr.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

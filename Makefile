##############################
# Makefile for ip6calc

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=

ip6calc: ip6calc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

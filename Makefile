CFLAGS = -std=c17 -pedantic -Wall
LDLIBS = -lncurses
CC = gcc

all: Chrono Funcs

Chrono:
	$(CC) Chrono.c -o Chrono $(CFLAGS) $(LDLIBS) 

Funcs:
	$(CC) Funcs
.c -o Funcs
 $(CFLAGS) $(LDLIBS)
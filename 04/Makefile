CC		= gcc
CFLAGS			+= -std=c99 -Wall -pedantic
TARGETS		=	02/matdump 03/mywc 01/utenti


01/1a: 01/1a.c
	$(CC) $(CFLAGS) $^ -o $@ 

02/1a: 02/1a.c
	$(CC) $(CFLAGS) $^ -o $@ 

03/1a: 03/1a.c
	$(CC) $(CFLAGS) $^ -o $@

04/1a: 04/1a.c
	$(CC) $(CFLAGS) $^ -o $@ 

.PHONY: all clean
.SUFFIXES: .c .h



all:
	01/1a 02/1a 03/1a 04/1a

clean: 
	-rm -f $(TARGETS)
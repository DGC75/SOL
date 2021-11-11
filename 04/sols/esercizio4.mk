## ---------------------------
## Esercizio 4 Assegnamento 4
##
## ---------------------------

CC		=  gcc
CFLAGS	        += -std=c99 -Wall -I ./utils/includes

TARGETS		= matdump        \
		  mywc           \
		  utenti

.PHONY: all clean
.SUFFIXES: .c .h

all	: $(TARGETS)

clean	: 
	-rm -f $(TARGETS)


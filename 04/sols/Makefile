CC		=  gcc
CFLAGS	        += -std=c99 -Wall
TARGETS		= matdump mywc utenti
INCLUDES	= -I ./

.PHONY: all clean cleanall
.SUFFIXES: .c .h

%: %.c
	$(CC) $(CFLAGS) $^ $(INCLUDES) -o $@ 

all		: $(TARGETS)

clean		: 
	-rm -f $(TARGETS)

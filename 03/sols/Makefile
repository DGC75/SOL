# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

# compilatore da usare
CC		=  gcc
# aggiungo alcuni flags di compilazione
CCFLAGS	        += -std=c99 -Wall
INCLUDES	= -I.
# librerie da linkare a tutti i files (es. -lm per le librerie matematiche)
LDFLAGS 	=
# flags di ottimizzazione 
OPTFLAGS	= # -O3
# flags di debugging
DBGFLAGS        = -g
FLAGS           = $(DBGFLAGS) $(OPTFLAGS)
ARFLAGS         = rvs
# aggiungere qui altri targets
TARGETS		= prog		 \
		  checkptr	 \
		  macro2         \
		  sommada

.PHONY: all clean cleanall tokenizer
.SUFFIXES: .c .h

# regola per la generazione di un eseguibile partendo da un .c
%: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(FLAGS) -o $@ $< $(LDFLAGS)

# regola per la generazione di un file oggetto partendo da un .c
%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(FLAGS) -c -o $@ $<


# root target
all		: $(TARGETS) tokenizer

# personalizzazione della generazione dell'eseguibile sommada
sommada:sommada.c
	$(CC) -DINIT_VALUE=3 $(CCFLAGS) $(INCLUDES) $(FLAGS) -o $@ $< $(LDFLAGS)

tokenizer: 
	@make -C tokenizer

clean		: 
	rm -f $(TARGETS)
	@make -C tokenizer clean
cleanall	: clean
	rm -f *.o *~
	@make -C tokenizer  cleanall




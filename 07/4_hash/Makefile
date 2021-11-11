CC = gcc
CFLAGS = -std=c99 -Wall -g -lpthread
DIRECTORIES =  ./hashLib

all: pipe hashLib

pipe: pipe.c hashLib/icl_hash.h hashLib
	gcc pipe.c -I ./hashLib/ -L ./hashLib -lHash -lpthread -Wl,-rpath,./hashLib/ -o $@

hashLib: hashLib/icl_hash.h hashLib/icl_hash.c
	gcc -Wall -I ./hashLib  ./hashLib/icl_hash.c -c -fPIC -o hashLib.o
	gcc -shared -o  ./hashLib/libHash.so hashLib.o
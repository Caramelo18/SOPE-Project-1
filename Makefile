CC = gcc

all: rmdup lsdir
rmdup: utilities.h rmdup.c
	cc rmdup.c -o bin/rmdup
lsdir: utilities.h lsdir.c
	cc lsdir.c -o bin/lsdir

CFLAGS = -Wall

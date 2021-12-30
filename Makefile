# TODO Move debug options to `make debug` or similar
a.out: wm.o
	gcc wm.o -lpanel -lncurses

wm.o: wm.c lib/headers/debug.h config.h
	gcc -D DEBUG -c wm.c
# TODO Move debug options to `make debug` or similar
a.out: wm.o lib/dll.o
	gcc wm.o lib/dll.o -lpanel -lncurses

wm.o: wm.c lib/headers/debug.h
	gcc -D DEBUG -c wm.c

lib/dll.o: lib/dll.c lib/dll.h
	gcc -c lib/dll.c -o lib/dll.o
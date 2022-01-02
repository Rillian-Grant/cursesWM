# TODO Move debug options to `make debug` or similar
a.out: wm.o
	gcc wm.o -lpanel -lncurses -ldl -g

wm.o: wm.c lib/headers/debug.h config.h
	gcc -D DEBUG -c wm.c -g

docs: wm.c lib/* doxygen.conf
	doxygen doxygen.conf

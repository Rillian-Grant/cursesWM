# TODO Move debug options to `make debug` or similar
a.out: wm.o blank_window_module win_move_module win_resize_module lib/menu/menu.h
	gcc wm.o -lpanel -lncurses -ldl -g

wm.o: wm.c lib/headers/debug.h config.h
	gcc -D DEBUG -c wm.c -g

menu.o: lib/menu/menu.c
	gcc -g -o lib/menu/menu.o lib/menu/menu.c -lncurses

blank_window_module: modules/types.h modules/blank_window/main.c
	gcc -D DEBUG -fPIC --shared -o modules/blank_window.so modules/blank_window/main.c -g
win_move_module: modules/types.h modules/win_move/main.c
	gcc -D DEBUG -fPIC --shared -o modules/win_move.so modules/win_move/main.c -g
win_resize_module: modules/types.h modules/win_resize/main.c
	gcc -D DEBUG -fPIC --shared -o modules/win_resize.so modules/win_resize/main.c -g


docs: wm.c lib/* doxygen.conf
	doxygen doxygen.conf

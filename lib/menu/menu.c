#include <ncurses.h>
#include "../headers/uthash.h"

#include "../headers/types.h"

MenuItem *menu;

void cwm_menu_refresh(MenuItem *currently_active_item) {
    // Top level menu
    move(0, 1);

    MenuItem *i, *tmp;
    HASH_ITER(hh, menu, i, tmp) {
        printw(i->name);
    }
}

MenuItem cwm_menu_init() {
    menu = NULL;


    MenuItem *top_menu = malloc(sizeof(MenuItem));
    strcpy(top_menu->name, "@");
    top_menu->event = 0;
    top_menu->submenu = NULL;

    MenuItem *system_menu = malloc(sizeof(MenuItem));
    strcpy(system_menu->name, "Exit");
    system_menu->event = 1;
    system_menu->submenu = NULL;

    HASH_ADD_STR(menu, name, top_menu);

    HASH_ADD_STR(menu->submenu, name, system_menu);
}

void cwm_menu_mode() {
;
}

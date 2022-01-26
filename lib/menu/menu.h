#ifndef MENU_H
#define MENU_H

#include <ncurses.h>
#include "../headers/uthash.h"

#include "../headers/types.h"
#include "../headers/debug.h"

void cwm_menu_refresh();
MenuItem cwm_menu_init();
bool cwm_menu_submenu_contains(MenuItem *menu_obj, MenuItem *item);
void cwm_menu_mode();


// ##################### Move to .c file later ####################

MenuItem *menu, *menu_category, *menu_item;
WINDOW *submenu;

MenuItem cwm_menu_init() {
    menu = NULL;


    MenuItem *at_menu = malloc(sizeof(MenuItem));
    strcpy(at_menu->name, "@");
    at_menu->event = 0;
    at_menu->submenu = NULL;

    MenuItem *about_item = malloc(sizeof(MenuItem));
    strcpy(about_item->name, "About");
    about_item->event = 2;
    about_item->submenu = NULL;

    MenuItem *exit_item = malloc(sizeof(MenuItem));
    strcpy(exit_item->name, "Exit");
    exit_item->event = 1;
    exit_item->submenu = NULL;

    HASH_ADD_STR(menu, name, at_menu);

    HASH_ADD_STR(menu->submenu, name, exit_item);
    HASH_ADD_STR(menu->submenu, name, about_item);

    MenuItem *system_menu = malloc(sizeof(MenuItem));
    strcpy(system_menu->name, "System");
    system_menu->event = 0;
    system_menu->submenu = NULL;

    MenuItem *system_menu_1 = malloc(sizeof(MenuItem));
    strcpy(system_menu_1->name, "Item 1");
    system_menu_1->event = 0;
    system_menu_1->submenu = NULL;

    MenuItem *system_menu_2 = malloc(sizeof(MenuItem));
    strcpy(system_menu_2->name, "A longer item 2");
    system_menu_2->event = 0;
    system_menu_2->submenu = NULL;

    MenuItem *system_menu_3 = malloc(sizeof(MenuItem));
    strcpy(system_menu_3->name, "The one that murders the puppies");
    system_menu_3->event = 0;
    system_menu_3->submenu = NULL;

    MenuItem *system_menu_4 = malloc(sizeof(MenuItem));
    strcpy(system_menu_4->name, "LOL");
    system_menu_4->event = 0;
    system_menu_4->submenu = NULL;

    HASH_ADD_STR(menu, name, system_menu);
    HASH_ADD_STR(system_menu->submenu, name, system_menu_1);
    HASH_ADD_STR(system_menu->submenu, name, system_menu_2);
    HASH_ADD_STR(system_menu->submenu, name, system_menu_3);
    HASH_ADD_STR(system_menu->submenu, name, system_menu_4);
}

void cwm_menu_mode() {
    debug_print("Entered menu mode\n");
    menu_category = menu;
    menu_item = NULL;
    cwm_menu_refresh();

    while (1) {
        int key = getch();

        debug_print("Before key press category is %s and item is %s\n", menu_category->name, menu_item->name);
        switch (key) {
            // LATER KEY_ENTER is wack
            case KEY_DOWN:
                if (menu_item->hh.next != NULL) menu_item = menu_item->hh.next;
                else menu_item = menu_category->submenu; // Reset to head
                break;
            case KEY_UP:
                if (menu_item->hh.prev != NULL) menu_item = menu_item->hh.prev;
                else while (menu_item->hh.next != NULL) menu_item = menu_item->hh.next;
                break;
            case KEY_RIGHT:
                if (menu_category->hh.next != NULL) menu_category = menu_category->hh.next;
                else menu_category = menu; // Reset to head
                menu_item = NULL;
                break;
            case KEY_LEFT:
                if (menu_category->hh.prev != NULL) menu_category = menu_category->hh.prev;
                else while (menu_category->hh.next != NULL) menu_category = menu_category->hh.next;
                menu_item = NULL;
                break;
            case KEY_HOME:
                goto outside_menu_mode_loop;
        }
        debug_print("After key press category is %s and item is %s\n", menu_category->name, menu_item->name);

        cwm_menu_refresh();
        debug_print("After refresh category is %s and item is %s\n", menu_category->name, menu_item->name);
    }
    outside_menu_mode_loop:;

    // Exit
    menu_category = menu_item = NULL;
    cwm_menu_refresh();
}

/* void cwm_menu_refresh(MenuItem *currently_active_submenu, MenuItem *currently_active_item) {
    debug_print("In referesh function\n");
    // Top level menu
    move(0, 1);
    MenuItem *i, *tmp;
    
    debug_print("Referesh\n");
    if (menu == NULL) debug_print("Menu is not initialized before iter\n");
    HASH_ITER(hh, menu, i, tmp) {
        debug_print("Inside iter\n");
        if (currently_active_item == currently_active_submenu) {
            ATTR_BLOCK(A_STANDOUT, {
                printw("%s", i->name);
            });

            // Display submenu
            int y, x;
            getyx(stdscr, y, x);
        } else {
            printw("%s", i->name);
        }
    }

    if (currently_active_item == NULL | currently_active_submenu == NULL) return;

    debug_print("Creating menu dropdown\n");

    int height, width;
    height = HASH_COUNT(currently_active_submenu) + 2;
    width = cwm_menu_submenu_name_maxlength(currently_active_submenu) + 2;

    WINDOW *menu_dropdown = newwin(height, width, 1, 1);
    wborder(menu_dropdown, 0, 0, ' ', 0, 0, 0, 0, 0);

    wmove(menu_dropdown, 1, 1);
    
    HASH_ITER(hh, currently_active_submenu->submenu, i, tmp) {
        if (i == currently_active_item) attron(A_STANDOUT);
        debug_print("Current submenu item: %p\n", i);
        wprintw(menu_dropdown, i->name);
        if (i == currently_active_item) attroff(A_STANDOUT);
    }


    wrefresh(menu_dropdown);

    // Clean up
}

int cwm_menu_submenu_name_maxlength(MenuItem *menu_item) {
    MenuItem *i, *tmp;
    int maxlength = 0;
    HASH_ITER(hh, menu_item->submenu, i, tmp) {
        int len = strlen(i->name);
        if (len > maxlength) maxlength = len;
    }

    return maxlength;
}

bool cwm_menu_submenu_contains(MenuItem *menu_obj, MenuItem *item) {
    debug_print("Menu item %p\n", item);
    if (menu_obj == NULL) return false;
    MenuItem *i, *tmp;
    
    HASH_ITER(hh, menu_obj, i, tmp) {
        debug_print("Current loop iteration %p\n", i);
        if (i == item) return true;
        else if (i->submenu != NULL) {
            MenuItem *i2, *tmp2;
            HASH_ITER(hh, i->submenu, i2, tmp2) {
                if (i == item) return true;
            }
        }
    }
    return false;
} */

// Works for two level menu
void cwm_menu_refresh() {
    // Clean up previous if needed
    cwm_menu_delete();

    debug_print("Starting refresh\n");

    MenuItem *i, *tmp; // For iteration
    
    // Refresh top line
    move(0, 1);
    HASH_ITER(hh, menu, i, tmp) {
        debug_print("Refreshing element %s in top line\n", i->name);
        int y, x;

        printw(" "); // Padding

        getyx(stdscr, y, x);
        i->y_position = y;
        i->x_start_position = x;

        if (i == menu_category) attron(A_STANDOUT);
        printw(i->name);
        if (i == menu_category) attroff(A_STANDOUT);

        getyx(stdscr, y, x);
        i->x_end_position = x;

        printw(" "); // Padding
    }

    if (menu_category == NULL) return;
    if (menu_item == NULL) menu_item = menu_category->submenu;

    /* // Find item and container
    MenuItem *active_submenu, *active_item;
    MenuItem *iter_toplevel_menu_item, *iter_submenu_item, *tmp2;
    HASH_ITER(hh, menu, iter_toplevel_menu_item, tmp) {
        debug_print("Current active iter_toplevel_menu_item is %p\n", iter_toplevel_menu_item);
        debug_print("Menu is %p\n", menu);
        if (iter_toplevel_menu_item == menu_current) {
            active_submenu = active_item = iter_toplevel_menu_item;
            debug_print("Current active active_item is %p\n", active_item);
            debug_print("Current active active_submenu is %p\n", active_submenu);
            break;
        } else if (iter_toplevel_menu_item->submenu != NULL) {
            HASH_ITER(hh, iter_toplevel_menu_item->submenu, iter_submenu_item, tmp2) {
                if (iter_submenu_item == menu_current) {
                    active_submenu = iter_toplevel_menu_item;
                    active_item = iter_submenu_item;
                    break;
                }
            }
        }
    } */

    // Now active_submenu and active_item are setS

    // Create menu
    int submenu_x, submenu_y, submenu_height, submenu_width;

    submenu_y = menu_category->y_position + 1;
    submenu_x = menu_category->x_start_position;
    //submenu_height = HASH_COUNT(menu_category->submenu) + 2;
    submenu_height = 0;
    submenu_width = 0;
    HASH_ITER(hh, menu_category->submenu, i, tmp) {
        i->x_start_position = 0;
        i->x_end_position = strlen(i->name);
        i->y_position = submenu_height;

        int len = i->x_end_position;
        if (len > submenu_width) submenu_width = len;

        submenu_height++;
    }
    submenu_width += 2; // For the border
    submenu_height += 2;

    submenu = newwin(submenu_height, submenu_width, submenu_y, submenu_x);
    box(submenu, 0, 0);

    HASH_ITER(hh, menu_category->submenu, i, tmp) {
        debug_print("Print loop\n");
        wmove(submenu, 1 + i->y_position, 1);
        debug_print("menu_category->submenu: %p\n", menu_category->submenu);
        debug_print("i: %p\n", i);
        if (i == menu_item) wattron(submenu, A_STANDOUT);
        wprintw(submenu, i->name);
        if (i == menu_item) wattroff(submenu, A_STANDOUT);
    }

    wrefresh(submenu);
}

void cwm_menu_delete() {
    if (submenu != NULL) {
        werase(submenu);
        wrefresh(submenu);
        delwin(submenu);

        submenu = NULL;
    }
}


#endif // MENU_H

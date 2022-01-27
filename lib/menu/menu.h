#ifndef MENU_H
#define MENU_H

#include <ncurses.h>
#include "../headers/utlist.h"

#include "../headers/types.h"
#include "../headers/debug.h"

void cwm_menu_refresh(MenuItem *menu);
MenuItem *cwm_menu_compose();
bool cwm_menu_submenu_contains(MenuItem *menu_obj, MenuItem *item);
int cwm_menu_mode(MenuItem *menu);


// ##################### Move to .c file later ####################

// TODO Replace with utlist

MenuItem *menu_category, *menu_item;
WINDOW *submenu;

MenuItem *cwm_menu_compose() {
    MenuItem *menu =  NULL;


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

    DL_APPEND(menu, at_menu);

    DL_APPEND(menu->submenu, exit_item);
    DL_APPEND(menu->submenu, about_item);

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

    DL_APPEND(menu, system_menu);
    DL_APPEND(system_menu->submenu, system_menu_1);
    DL_APPEND(system_menu->submenu, system_menu_2);
    DL_APPEND(system_menu->submenu, system_menu_3);
    DL_APPEND(system_menu->submenu, system_menu_4);

    return menu;
}

int cwm_menu_mode(MenuItem *menu) {
    debug_print("Entered menu mode\n");
    menu_category = menu;
    menu_item = NULL;
    int event = 0;
    
    cwm_menu_refresh(menu);

    while (1) {
        int key = getch();

        debug_print("Before key press category is %s and item is %s\n", menu_category->name, menu_item->name);
        switch (key) {
            // LATER KEY_ENTER is wack
            case KEY_DOWN:
                if (menu_item->next != NULL) menu_item = menu_item->next;
                else menu_item = menu_category->submenu; // Reset to head
                break;
            case KEY_UP:
                if (menu_item->prev != NULL) menu_item = menu_item->prev;
                else while (menu_item->next != NULL) menu_item = menu_item->next;
                break;
            case KEY_RIGHT:
                if (menu_category->next != NULL) menu_category = menu_category->next;
                else menu_category = menu; // Reset to head
                menu_item = NULL;
                break;
            case KEY_LEFT:
                if (menu_category->prev != NULL) menu_category = menu_category->prev;
                else while (menu_category->next != NULL) menu_category = menu_category->next;
                menu_item = NULL;
                break;
            case KEY_HOME:
                goto outside_menu_mode_loop;
                break;
            case 10: // Enter, for some reason '\n' '\r' and KEY_ENTER are all wrong
                debug_print("Item %s selected returning with event %i\n", menu_item->name, menu_item->event);
                if (menu_item != NULL) event = menu_item->event;
                else event = 0;
                goto outside_menu_mode_loop;
                break;
        }
        debug_print("After key press category is %s and item is %s\n", menu_category->name, menu_item->name);

        cwm_menu_refresh(menu);
        debug_print("After refresh category is %s and item is %s\n", menu_category->name, menu_item->name);
    }
    outside_menu_mode_loop:;

    // Exit
    menu_category = menu_item = NULL;
    cwm_menu_refresh(menu);
    return event;
}

// Works for two level menu
void cwm_menu_refresh(MenuItem *menu) {
    // Clean up previous if needed
    cwm_menu_delete();

    debug_print("Starting refresh\n");

    MenuItem *i; // For iteration
    
    // Refresh top line
    move(0, 1);
    if (menu == NULL) clrtoeol();
    DL_FOREACH(menu, i) {
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

    // Now active_submenu and active_item are setS

    // Create menu
    int submenu_x, submenu_y, submenu_height, submenu_width;

    submenu_y = menu_category->y_position + 1;
    submenu_x = menu_category->x_start_position;
    //submenu_height = HASH_COUNT(menu_category->submenu) + 2;
    submenu_height = 0;
    submenu_width = 0;
    DL_FOREACH(menu_category->submenu, i) {
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

    DL_FOREACH(menu_category->submenu, i) {
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

/**
 * @file wm.c
 * @author Rillian Grant (rillian.grant@gmail.com)
 * @brief Main file for the window manager
 * @version 0.1
 * @date 2022-01-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <ncurses.h>
#include <panel.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "lib/headers/debug.h"
#include "lib/headers/types.h"
#include "lib/headers/utlist.h"
#include <linux/limits.h>
#include <sys/types.h>

#include "lib/menu/menu.h"

typedef struct Module_list_item Module_list_item;

/**
 * @brief Doubly linked list node structure that contains a module and that module's custom data.
 * 
 */
typedef struct Module_list_item {
    Module *module;                 ///< The module. This is declared in types.h and defined in the module in question.
    void **data;                    ///< A pointer to the modules's custom pointer.
    Module_list_item *next;
    Module_list_item *prev;
} Module_list_item;

// Function declarations
PANEL *cwm_window_create(int height, int width, int y, int x);
void cwm_window_status(PANEL *panel, int status);
Module_list_item *load_modules();
Module *load_module(char *filepath);
int handle_event(int event, void *event_data);
int cwm_window_move(PANEL *panel, int y, int x);

// Holds a pointer to the current window. The panel library handles getting other windows.
PANEL *current_win;
Module_list_item *modules; // Head of the list of modules.
// Shorthand for getting the window's associated CWM_WINDOW_DATA object.
#define cwm_window_data(win) ((CWM_WINDOW_DATA *)panel_userptr(win))

int main() {
    debug_setup();
    debug_print("Started\n");

    // Initalise ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    keypad(stdscr, TRUE);
    mousemask(BUTTON1_PRESSED, NULL);
    debug_print("Finished setting up ncurses.\n");

    // Load modules
    modules = load_modules();
    // DL_COUNT requires several variables that it can modify to function.
    Module_list_item *el;
    int count;
    DL_COUNT(modules, el, count);
    debug_print("Num of modules loaded: %i\n", count);


    // Main loop

    int key;
    while (1) {
        MenuItem *m;
        if (current_win != NULL) m = cwm_window_data(current_win)->menu; // TODO
        else m = NULL;
        cwm_menu_refresh(m); // TODO
        debug_print("Waiting for input...\n");

        key = getch();
        
        debug_print("Key with integer representation %i was pressed. Displayed as %c (n.b. This could be wrong)\n", key, key);

        #ifdef DEBUG
        if (key == KEY_MOUSE) { // TODO This is a test. Expand later.
            MEVENT event;
            getmouse(&event); // OK?
            debug_print("Mouse event recorded. Button 1 pressed: %i. Coords (%i, %i, %i)\n", (event.bstate & BUTTON1_PRESSED), event.x, event.y, event.z);
        }
        #endif

        if (key == KEY_HOME) { // Home key
            int menu_event = cwm_menu_mode(m); // TODO
            touchwin(panel_window(current_win)); // Force panel update. Because panel library doesn't know about the menu.
            
            if (menu_event != 0) {
                handle_event(menu_event, NULL);
            }

            update_panels();
            doupdate();
            continue;
        }

        // Run module event handlers
        bool module_effected_state = handle_event(MODULE_EVENT_KEY_PRESS, &key);

        // Switch between windows
        if ( key == KEY_TAB) {
            // TODO Change window header to show top

            current_win = panel_above((PANEL *)0);
            top_panel(current_win);



            update_panels();
            doupdate();
        }

        if (current_win == NULL || module_effected_state == false) continue;

        //<resize_do>
        // WARNING recreating windows with resize allows moving outside of screen and causes undefined behaviour
        if (
            cwm_window_data(current_win)->height != cwm_window_data(current_win)->_height
            | cwm_window_data(current_win)->width != cwm_window_data(current_win)->_width
        ) {
            if (
                    cwm_window_data(current_win)->height > CONFIG_WINDOW_HEIGHT_MINIMUM                 // Height minimum
                &   cwm_window_data(current_win)->width > CONFIG_WINDOW_WIDTH_MINIMUM                   // Width minimum
                &   cwm_window_data(current_win)->width + cwm_window_data(current_win)->x_pos < COLS    // Width on screen
                &   cwm_window_data(current_win)->height + cwm_window_data(current_win)->y_pos < LINES  // Height on screen
            ) {
                // Resizing the windows is done by creating a new window and switching them out
                WINDOW *old_win = panel_window(current_win);
                WINDOW *new_win = newwin(cwm_window_data(current_win)->height, cwm_window_data(current_win)->width, cwm_window_data(current_win)->y_pos, cwm_window_data(current_win)->x_pos);
                replace_panel(current_win, new_win);
                box(panel_window(current_win), 0, 0);
                delwin(old_win);
                cwm_window_data(current_win)->associated_ncurses_window = new_win;

                cwm_window_data(current_win)->_height = cwm_window_data(current_win)->height;
                cwm_window_data(current_win)->_width = cwm_window_data(current_win)->width;

                handle_event(MODULE_EVENT_WIN_RECREATED, NULL);
            } else {
                cwm_window_data(current_win)->height = cwm_window_data(current_win)->_height;
                cwm_window_data(current_win)->width = cwm_window_data(current_win)->_width;
            }
        }
        //</resize_do>

        //<move_do>
        if (
            cwm_window_data(current_win)->x_pos != cwm_window_data(current_win)->_x_pos
            | cwm_window_data(current_win)->y_pos != cwm_window_data(current_win)->_y_pos
        ) {
            if (
                    cwm_window_data(current_win)->y_pos > 0 // Disallow top row as that contains the menu bar
                &   cwm_window_data(current_win)->x_pos >= 0
                &   cwm_window_data(current_win)->y_pos + cwm_window_data(current_win)->height <= LINES
                &   cwm_window_data(current_win)->x_pos + cwm_window_data(current_win)->width <= COLS
            ) {
                cwm_window_move(current_win, cwm_window_data(current_win)->y_pos, cwm_window_data(current_win)->x_pos);

                cwm_window_data(current_win)->_x_pos = cwm_window_data(current_win)->x_pos;
                cwm_window_data(current_win)->_y_pos = cwm_window_data(current_win)->y_pos;
            } else {
                cwm_window_data(current_win)->x_pos = cwm_window_data(current_win)->_x_pos;
                cwm_window_data(current_win)->y_pos = cwm_window_data(current_win)->_y_pos;
            }
        }
        //</move_do>

        // Updates what's on screen. This should only be run here
        update_panels();
        doupdate();
    }

    endwin();

    debug_shutdown();
}

/**
 * @brief Handles an event by calling the associated modules and reacting appropriately to their outputs.
 * 
 * @param event The event to be handled.
 * @return int 0 If no window state was changed, 1 if it was.
 */
int handle_event(int event, void *event_data) {
    Module_list_item *m;
    bool module_effected_state = false;
    DL_FOREACH(modules, m) {
        bool module_finished = false; // Is set to true when a module returns something that is not command
        while (!module_finished) {
            debug_print("Running module %s with event %i\n", m->module->name, event);
            int ret = m->module->handler(event, event_data, m->data, cwm_window_data(current_win));
            debug_print("Module done returning %i\n", ret);

            if (ret == MODULE_RETURN_DONE) {
                module_effected_state = true;
                module_finished = true;
            }
            else if (ret == MODULE_RETURN_NOP) {
                module_finished = true;
            }
            else if (ret == MODULE_RETURN_WIN_NEW) {
                current_win = cwm_window_create(10, 55, 7, 55);
                cwm_window_data(current_win)->associated_module = m->module;
                cwm_window_data(current_win)->module_data = malloc(sizeof(void *));

                module_effected_state = true;
                event = MODULE_EVENT_WIN_CREATED;
            }
            else if (ret == MODULE_RETURN_WIN_DEL) {
                PANEL *to_delete = current_win;
                current_win = panel_below(current_win);
                free((void *)panel_userptr(to_delete));
                del_panel(to_delete);
                module_effected_state = true;
                module_finished = true;
            }
        }
        
    }

    if (module_effected_state) return 1;
    else return 0;
}

int cwm_window_move(PANEL *panel, int y, int x) {
    return move_panel(panel, y, x);
}

/**
 * @brief Creates a new window and initializes the associated data structures in the userptr.
 * 
 * @param height    Height of the new window.
 * @param width     Width of the new window.
 * @param y         Y coord of the top left corner of the window. This to counting from the top of the screen.
 * @param x         X coord of the top left corner of the window.
 * @return          PANEL* Pointer to the newly created window
 */
PANEL *cwm_window_create(int height, int width, int y, int x) {
    WINDOW *win = newwin(height, width, y, x);
    box(win, 0, 0);
    PANEL *pan = new_panel(win);
    CWM_WINDOW_DATA *obj = malloc(sizeof(CWM_WINDOW_DATA));
    obj->x_pos = obj->_x_pos = x;
    obj->y_pos = obj->_y_pos = y;
    obj->height = obj->_height = height;
    obj->width = obj->_width = width;
    obj->status = obj->_status = 0;
    obj->associated_ncurses_window = win;
    obj->menu = NULL;

    set_panel_userptr(pan, obj);

    return pan;
}

/**
 * @brief Currently unused function.
 * 
 * @param panel 
 * @param status 
 */
void cwm_window_status(PANEL *panel, int status) {
    ((CWM_WINDOW_DATA *)panel_userptr(panel))->status = status;

    wborder(panel_window(panel), 0, 0, 0, 0, status, 0, 0, 0);
}

/**
 * @brief Loads modules from a folder in the current directory named modules.
 * @todo Make more resilient.
 * 
 * @return Module_list_item* The head of the double linked list of modules.
 */
Module_list_item *load_modules() {
    #define MODULE_DIR "./modules" // Must not end with a trailing slash. TODO Fix
    DIR *module_dir = opendir(MODULE_DIR);

    struct dirent *current_file;

    char *extension;

    char filepath[PATH_MAX + 1];
    char filepath_real[PATH_MAX + 1];

    Module_list_item *module_list_head = NULL;

    // Loop through files in the module directory
    while ((current_file = readdir(module_dir)) != NULL) {
        extension = strrchr(current_file->d_name, '.'); // Returns a pointer to the '.' in the filename
        // If the file has an extension and is not a hidden file and the extension is '.so'
        if(extension && extension != current_file->d_name && !strcmp(extension, ".so")) {
            sprintf(filepath, "%s/%s", MODULE_DIR, current_file->d_name); // sets filepath to MODULE_DIR/filename
            debug_print("Combined path: %s\n", filepath);
            realpath(filepath, filepath_real); // Gets the fill path of the module
            debug_print("Real path: %s\n", filepath_real);
            
            // Load the module and initialise the custom data pointer
            Module_list_item *new_module_list_item = malloc(sizeof(Module_list_item));
            new_module_list_item->module = load_module(filepath_real);
            new_module_list_item->data = malloc(sizeof(void *));
            *new_module_list_item->data = NULL;

            debug_print("Loaded module: %s\n", new_module_list_item->module->name);
            DL_APPEND(module_list_head, new_module_list_item);
        }
    }

    return module_list_head;
}

/**
 * @brief Loads a single module
 * 
 * @param filepath Path to the module file
 * @return Module* Module loaded object
 */
Module *load_module(char *filepath) {
    char *error;

    void *module = dlopen(filepath, RTLD_NOW);
    if (!module) {
        debug_print("FATAL: Cannot load module: %s\n", dlerror());
        endwin(); exit(1);
    }

    Module *info;
    info = dlsym(module, "module");
    error = dlerror();
    if (error) {
        debug_print("FATAL: Could not load module at: %s. Error: %s\n", filepath, error);
        endwin(); exit(1);
    }

    return info;
}

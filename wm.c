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

typedef struct Module_list_item Module_list_item;
typedef struct Module_list_item {
    Module *module;
    void **data;
    Module_list_item *next;
    Module_list_item *prev;
} Module_list_item;

PANEL *cwm_window_create(int height, int width, int y, int x);
void cwm_window_status(PANEL *panel, int status);
Module_list_item *load_modules();
Module *load_module(char *filepath);

PANEL *current_win;
#define cwm_window_data(win) ((CWM_WINDOW_DATA *)panel_userptr(win))

int main() {
    debug_setup();
    debug_print("Starting...\n");

    // Init
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    debug_print("Finished setting up ncurses.\n");

    // Load modules
    Module_list_item *modules = load_modules();
    Module_list_item *el;
    int count;
    DL_COUNT(modules, el, count);
    debug_print("Num of modules: %i\n", count);

    // ##############

    int key;
    while (1) {
        debug_print("Waiting for input...\n");

        key = getch();
        
        debug_print("Key with integer representation %i was pressed. Displayed as %c (n.b. This could be wrong)\n", key, key);

        // Run modules
        Module_list_item *m;
        bool done_something = false;
        DL_FOREACH(modules, m) {
            bool module_finished = false; // Is set to true when a module returns something that is not command
            int event = MODULE_EVENT_KEY_PRESS;
            while (!module_finished) {
                debug_print("Running module %s with event %i\n", m->module->name, event);
/*                 if (current_win != NULL) wprintw(cwm_window_data(current_win)->associated_ncurses_window, "Hello");
                update_panels(); doupdate(); */
                int ret = m->module->handler(event, &key, m->data, cwm_window_data(current_win));
                debug_print("Module done returning %i\n", ret);

                if (ret == MODULE_RETURN_DONE) {
                    done_something = true;
                    module_finished = true;
                }
                else if (ret == MODULE_RETURN_NOP) {
                    module_finished = true;
                }
                else if (ret == MODULE_RETURN_WIN_NEW) {
                    current_win = cwm_window_create(10, 55, 7, 55);
                    cwm_window_data(current_win)->associated_module = m->module;
                    done_something = true;
                    event = MODULE_EVENT_WIN_CREATED;
                }
                else if (ret == MODULE_RETURN_WIN_DEL) {
                    PANEL *to_delete = current_win;
                    current_win = panel_below(current_win);
                    free((void *)panel_userptr(to_delete));
                    del_panel(to_delete);
                    done_something = true;
                    module_finished = true;
                }
            }
            
        }


        if ( key == KEY_TAB) {
            current_win = panel_above((PANEL *)0);
            top_panel(current_win);
            update_panels();
            doupdate();
        }

debug_print("Before check\n");
        if (current_win == NULL || done_something == false) continue;
debug_print("After check\n");


        //<resize_do>
        if (
            cwm_window_data(current_win)->height != cwm_window_data(current_win)->_height
            | cwm_window_data(current_win)->width != cwm_window_data(current_win)->_width
        ) {
            WINDOW *old_win = panel_window(current_win);
            WINDOW *new_win = newwin(cwm_window_data(current_win)->height, cwm_window_data(current_win)->width, cwm_window_data(current_win)->y_pos, cwm_window_data(current_win)->x_pos);
            replace_panel(current_win, new_win);
            box(panel_window(current_win), 0, 0);
            delwin(old_win);
            cwm_window_data(current_win)->associated_ncurses_window = new_win;
            update_panels();
            doupdate();
        }
        //</resize_do>

        //<move_do>
        if (
            cwm_window_data(current_win)->x_pos != cwm_window_data(current_win)->_x_pos
            | cwm_window_data(current_win)->y_pos != cwm_window_data(current_win)->_y_pos
        ) {
            move_panel(current_win, cwm_window_data(current_win)->y_pos, cwm_window_data(current_win)->x_pos);
        }
        //</move_do>

        update_panels();
        doupdate();
    }

    endwin();

    debug_shutdown();
}

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

    set_panel_userptr(pan, obj);

    update_panels();
    doupdate();

    return pan;
}

void cwm_window_status(PANEL *panel, int status) {
    ((CWM_WINDOW_DATA *)panel_userptr(panel))->status = status;

    wborder(panel_window(panel), 0, 0, 0, 0, status, 0, 0, 0);

    update_panels();
    doupdate();
}

Module_list_item *load_modules() {
    #define MODULE_DIR "./modules" // Must not end with a trailing slash. TODO Fix
    DIR *module_dir = opendir(MODULE_DIR);

    struct dirent *current_file;

    char *extension;

    char filepath[PATH_MAX + 1];
    char filepath_real[PATH_MAX + 1];

    Module_list_item *module_list_head = NULL;

    while ((current_file = readdir(module_dir)) != NULL) {
        extension = strrchr(current_file->d_name, '.');
        if(extension && extension != current_file->d_name && !strcmp(extension, ".so")) {
            sprintf(filepath, "%s/%s", MODULE_DIR, current_file->d_name);
            debug_print("Combined path: %s\n", filepath);
            realpath(filepath, filepath_real);
            debug_print("Real path: %s\n", filepath_real);
            Module_list_item *new_module_list_item = malloc(sizeof(Module_list_item));
            new_module_list_item->module = load_module(filepath_real);
            new_module_list_item->data = malloc(sizeof(void *));
            *new_module_list_item->data = NULL;
            debug_print("Loading module: %s\n", new_module_list_item->module->name);
            DL_APPEND(module_list_head, new_module_list_item);
        }
    }

    return module_list_head;
}

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

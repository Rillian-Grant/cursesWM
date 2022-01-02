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
#include <limits.h>
#include <sys/types.h>

typedef struct Module_list_item Module_list_item;
typedef struct Module_list_item {
    Module *module;
    Module_list_item *next;
    Module_list_item *prev;
} Module_list_item;

PANEL *cwm_window_create(int height, int width, int y, int x);
void cwm_window_move(PANEL *panel, int y, int x);
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

    // ##############

    int key;
    while (1) {
        debug_print("Waiting for input...\n");

        key = getch();
        
        debug_print("Key with integer representation %i was pressed. Displayed as %c (n.b. This could be wrong)\n", key, key);

        // Run modules
        debug_print("Running module...\n");
        int ret = modules->module->handler(MODULE_EVENT_KEY_PRESS, &key, NULL, cwm_window_data(current_win));
        debug_print("Module done returning %i\n", ret);

        if (key == ctrl_letter('n')) {
            PANEL *new_window = cwm_window_create(10, 55, 7, 55);
            current_win = new_window;
            update_panels();
            doupdate();
        } else if ( key == ctrl_letter('d')) {
            PANEL *to_delete = current_win;
            current_win = panel_below(current_win);
            free((void *)panel_userptr(to_delete)); // Discard const qualifier from pointer type
            del_panel(to_delete);
            update_panels();
            doupdate();
        } else if ( key == KEY_TAB) {
            current_win = panel_above((PANEL *)0);
            top_panel(current_win);
            update_panels();
            doupdate();
        }

        if (current_win == NULL) continue;

        // Resize
        else if ( key == KEY_ALT_UP) cwm_window_data(current_win)->height--;
        else if ( key == KEY_ALT_DOWN) cwm_window_data(current_win)->height++;
        else if ( key == KEY_ALT_LEFT) cwm_window_data(current_win)->width -= 2; // Console font ratio is 2 to 1
        else if ( key == KEY_ALT_RIGHT) cwm_window_data(current_win)->width += 2;

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
            update_panels();
            doupdate();
        }
        //</resize_do>

        if (
            cwm_window_data(current_win)->x_pos != cwm_window_data(current_win)->_x_pos
            | cwm_window_data(current_win)->y_pos != cwm_window_data(current_win)->_y_pos
        ) {
            cwm_window_move(current_win, cwm_window_data(current_win)->y_pos, cwm_window_data(current_win)->x_pos);
        }
    }

    endwin();

    debug_shutdown();
}

PANEL *cwm_window_create(int height, int width, int y, int x) {
    WINDOW *win = newwin(height, width, y, x);
    box(win, 0, 0);
    PANEL *pan = new_panel(win);
    CWM_WINDOW_DATA *obj = malloc(sizeof(CWM_WINDOW_DATA));
    obj->x_pos = x;
    obj->y_pos = y;
    obj->height = height;
    obj->width = width;
    obj->status = 0;

    set_panel_userptr(pan, obj);

    update_panels();
    doupdate();

    return pan;
}

void cwm_window_move(PANEL *panel, int y, int x) {
    move_panel(panel, y, x);

    update_panels();
    doupdate();
}

void cwm_window_status(PANEL *panel, int status) {
    ((CWM_WINDOW_DATA *)panel_userptr(panel))->status = status;

    wborder(panel_window(panel), 0, 0, 0, 0, status, 0, 0, 0);

    update_panels();
    doupdate();
}

Module_list_item *load_modules() {
    DIR *module_dir = opendir("./modules");

    struct dirent *current_file;

    char *extension;

    char filepath[PATH_MAX + 1];

    Module_list_item *module_list_head = NULL;

    while ((current_file = readdir(module_dir)) != NULL) {
        extension = strrchr(current_file->d_name, '.');
        if(extension && extension != current_file->d_name && !strcmp(extension, ".so")) {
            realpath(current_file->d_name, filepath);
            debug_print("%s\n", filepath);
            Module_list_item *new_module_list_item = malloc(sizeof(Module_list_item));
            new_module_list_item->module = load_module(filepath);
            module_list_head = new_module_list_item;
        }
    }

    return module_list_head;
}

Module *load_module(char *filepath) {
    char *error;

    void *module = dlopen("/home/rillian/Programming/cursesWM/modules/win_move.so", RTLD_NOW);
    if (!module) {
        debug_print("FATAL: Cannot load module: %s\n", dlerror());
        exit(1);
    }

    Module *info;
    info = dlsym(module, "info");
    error = dlerror();
    if (error) {
        debug_print("FATAL: Could not load module at: %s. Error: %s\n", filepath, error);
        exit(1);
    }

    return info;
}

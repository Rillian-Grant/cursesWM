#include <ncurses.h>
#include <panel.h>
#include <dlfcn.h>

#include "config.h"
#include "lib/headers/debug.h"
#include "lib/headers/ut/uthash.h"
#include "lib/headers/keycodes.h"
#include "modules/types.h"

PANEL *cwm_window_create(int height, int width, int y, int x);
void cwm_window_move(PANEL *panel, int y, int x);

PANEL *current_win;                                 /**< Holds the currently focused window */
#define cwm_window_data(win) ((CWM_WINDOW_DATA *)panel_userptr(win))

int main() {
    debug_setup();

    // Initialise ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    debug_print("Finished setting up ncurses.\n");


// ##############################################

    // Load modules
    void *win_blank = dlopen("/home/rillian/Programming/cursesWM/modules/win_blank.so", RTLD_NOW);
    if (!win_blank)
  {
     debug_print("Cannot load module: %s\n", dlerror ());
     exit(1);
  }
    typedef int(*handle_f)(PANEL *, int);
    handle_f win_blank_handle;
    win_blank_handle = dlsym(win_blank, "handle");
    char *result = dlerror();
    if (result)
{
   debug_print("Cannot find init in module: %s\n", result);
   exit(1);
}

// ##############################################

    int key;
    #define no_mode 0
    #define edit_mode 1
    #define move_mode 2
    #define resize_mode 3
    int mode = no_mode;
    while (1) {
        debug_print("Mode %i\n", mode);
        debug_print("Waiting for input...\n");

        key = getch();
        
        debug_print("Key with integer representation %i was pressed. Displayed as %c (n.b. This could be wrong)\n", key, key);

        int res = win_blank_handle(current_win, key);
        if (res == RETURN_NEW_WIN) {
            int height = 10;
            int width = 55;
            int x = 7;
            int y = 55;

            WINDOW *win = newwin(height, width, y, x);
            box(win, 0, 0);
            PANEL *pan = new_panel(win);
            CWM_WINDOW_DATA *obj = malloc(sizeof(CWM_WINDOW_DATA));
            obj->x_pos = obj->_x_pos = x;
            obj->y_pos = obj->_y_pos = y;
            obj->height = height;
            obj->width = width;
            obj->status = 0;

            set_panel_userptr(pan, obj);

            current_win = pan;

            win_blank_handle(current_win, key);
        }
        update_panels();
        doupdate();

        debug_print("Is current_win null: %i\n", (current_win == NULL));

        // <event_block>
/*         if (key == ctrl_letter('n')) {
            PANEL *new_window = cwm_window_create(10, 55, 7, 55);
            current_win = new_window;
        }
        else if (key == ctrl_letter('d')) {
            ;
            PANEL *to_delete = current_win;
            current_win = panel_below(current_win);
            free((void *)panel_userptr(to_delete)); // Discard const qualifier from pointer type
            del_panel(to_delete);
            update_panels();
            doupdate();
        }
        else */ if (key == KEY_TAB) {
            current_win = panel_above((PANEL *)0);
            top_panel(current_win);
        }
        else if (key == KEY_ALT_UP) {
            cwm_window_data(current_win)->height--;
        }
        else if (key == KEY_ALT_DOWN) {
            cwm_window_data(current_win)->height++;
        }
        else if (key == KEY_ALT_LEFT) {
            cwm_window_data(current_win)->width -= 2; // Console font ratio is 2 to 1
        }
        else if (key == KEY_ALT_RIGHT) {
            cwm_window_data(current_win)->width += 2;
        }
        else if (key == KEY_CTRL_UP) {
            cwm_window_data(current_win)->y_pos--;
        }
        else if (key == KEY_CTRL_DOWN) {
            cwm_window_data(current_win)->y_pos++;
        }
        else if (key == KEY_CTRL_LEFT) {
            cwm_window_data(current_win)->x_pos -= 2; // Console font ratio is 2 to 1
        }
        else if (key == KEY_CTRL_RIGHT) {
            cwm_window_data(current_win)->x_pos += 2;
        } 
        
        /* else if (key == KEY_BACKSPACE) {
            debug_print("Backspace\n");
            cwm_window_data(current_win)->window_buffer[cwm_window_data(current_win)->window_buffer_end_pos - 1] = ' ';
            cwm_window_data(current_win)->window_buffer_end_pos--;
        }
        else {
            debug_print("Key: %c\n", key);
            cwm_window_data(current_win)->window_buffer[cwm_window_data(current_win)->window_buffer_end_pos] = key;
            cwm_window_data(current_win)->window_buffer_end_pos++;
        } */
        // </event_block>

        if (current_win == NULL) continue;

        // <resize_final>
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

            cwm_window_data(current_win)->_height = cwm_window_data(current_win)->height;
            cwm_window_data(current_win)->_width = cwm_window_data(current_win)->width;
        }
        // </resize_final>

        // <move_final>
        if (
            cwm_window_data(current_win)->x_pos != cwm_window_data(current_win)->_x_pos
            | cwm_window_data(current_win)->y_pos != cwm_window_data(current_win)->_y_pos
        ) {
            cwm_window_move(current_win, cwm_window_data(current_win)->y_pos, cwm_window_data(current_win)->x_pos);

            cwm_window_data(current_win)->_x_pos = cwm_window_data(current_win)->x_pos;
            cwm_window_data(current_win)->_y_pos = cwm_window_data(current_win)->y_pos;
        }
        // </move_final>

        update_panels();
        doupdate();

        debug_print("\n\n");
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

// Currently unused
void cwm_window_status(PANEL *panel, int status) {
    ((CWM_WINDOW_DATA *)panel_userptr(panel))->status = status;

    wborder(panel_window(panel), 0, 0, 0, 0, status, 0, 0, 0);

    update_panels();
    doupdate();
//    wrefresh(cwm_win->window);
}
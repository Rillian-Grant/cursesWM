#include <ncurses.h>
#include <panel.h>

#include "config.h"
#include "lib/headers/debug.h"
#include "lib/headers/keycodes.h"

/**
 * @brief Contains all window data
 */
typedef struct {
    int x_pos, _x_pos;
    int y_pos, _y_pos;
    int height, _height;
    int width, _width;
    char status;
} CWM_WINDOW_DATA;

PANEL *cwm_window_create(int height, int width, int y, int x);
void cwm_window_move(PANEL *panel, int y, int x);
void cwm_window_status(PANEL *panel, int status);

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

    int key;
    while (1) {
        debug_print("Mode %i\n", mode);
        debug_print("Waiting for input...\n");

        key = getch();
        
        debug_print("Key with integer representation %i was pressed. Displayed as %c (n.b. This could be wrong)\n", key, key);


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

        // Move
        else if ( key == KEY_CTRL_UP) cwm_window_data(current_win)->y_pos--;
        else if ( key == KEY_CTRL_DOWN) cwm_window_data(current_win)->y_pos++;
        else if ( key == KEY_CTRL_LEFT) cwm_window_data(current_win)->x_pos -= 2; // Console font ratio is 2 to 1
        else if ( key == KEY_CTRL_RIGHT) cwm_window_data(current_win)->x_pos += 2;

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
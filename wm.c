#include <ncurses.h>
#include <panel.h>

#include "lib/headers/debug.h"
#include "lib/dll.h"

/**
 * @brief Contains all window data.
 */
typedef struct {
    char window_buffer[1000];       /**< Primary source for the text the window contains. Wrap around when you reach the end. Size could be hardcoded, or changed on startup with config files. Allocating swap is too much work */
    int window_buffer_position;     /**< Window buffer position to next write to. Also where to start reading from */
    int x_pos;
    int y_pos;
    int height;
    int width;
    char status;
} CWM_WINDOW_DATA;

PANEL *cwm_window_create(int height, int width, int y, int x);
void cwm_window_move(PANEL *panel, int y, int x);
void cwm_window_refresh(PANEL *panel);
void cwm_window_status(PANEL *panel, int status);

DLL *cwm_windows_new(DLL *current_window, int height, int width, int y, int x);

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

    PANEL *other_win = malloc(sizeof(PANEL));
    PANEL *current_win = malloc(sizeof(PANEL));
    other_win = cwm_window_create(10, 55, 5, 50);
    current_win = cwm_window_create(10, 100, 0, 0);

    // TODO Refresh makes window top for some reason
    cwm_window_refresh(other_win);
    cwm_window_refresh(current_win);

    // Used to swap
    PANEL *oc;

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

        if (mode == no_mode) {
            debug_print("In no mode\n");
            switch (key)
            {
            case 'e':
                mode = edit_mode;
                cwm_window_status(current_win, 'e');
                break;
            case 'm':
                mode = move_mode;
                cwm_window_status(current_win, 'm');
                break;
            case 'r':
                mode = resize_mode;
                cwm_window_status(current_win, 'r');
                break;
            case 9:
                oc = current_win;
                current_win = other_win;
                other_win = oc;
                top_panel(current_win);
                cwm_window_refresh(current_win);
                break;
            default:
                break;
            }
        }

        if (mode == resize_mode) {
            switch (key)
                {
                case KEY_UP:
                    ((CWM_WINDOW_DATA *)panel_userptr(current_win))->height--;
                    break;
                case KEY_DOWN:
                    ((CWM_WINDOW_DATA *)panel_userptr(current_win))->height++;
                    break;
                case KEY_LEFT:
                    ((CWM_WINDOW_DATA *)panel_userptr(current_win))->width -= 2; // Console font ratio is 2 to 1
                    break;
                case KEY_RIGHT:
                    ((CWM_WINDOW_DATA *)panel_userptr(current_win))->width += 2;
                    break;
                default:
                    break;
                }

            WINDOW *old_win = panel_window(current_win);
            WINDOW *new_win = newwin(((CWM_WINDOW_DATA *)panel_userptr(current_win))->height, ((CWM_WINDOW_DATA *)panel_userptr(current_win))->width, ((CWM_WINDOW_DATA *)panel_userptr(current_win))->y_pos, ((CWM_WINDOW_DATA *)panel_userptr(current_win))->x_pos);
            replace_panel(current_win, new_win);
            box(panel_window(current_win), 0, 0);
            delwin(old_win);
            cwm_window_refresh(current_win);
            update_panels();
            doupdate();
            
            if (key == KEY_END) {
                mode = no_mode;
                cwm_window_status(current_win, 0);
            }
        }

        if (mode == move_mode) {
            debug_print("In move mode\n");
            if (
                key == KEY_UP
                || key == KEY_DOWN
                || key == KEY_LEFT
                || key == KEY_RIGHT
            ) {

            debug_print("Moving window from (%i, %i)...\n", ((CWM_WINDOW_DATA *)panel_userptr(current_win))->x_pos, ((CWM_WINDOW_DATA *)panel_userptr(current_win))->y_pos);

            switch (key)
                {
                case KEY_UP:
                    ((CWM_WINDOW_DATA *)panel_userptr(current_win))->y_pos--;
                    break;
                case KEY_DOWN:
                    ((CWM_WINDOW_DATA *)panel_userptr(current_win))->y_pos++;
                    break;
                case KEY_LEFT:
                    ((CWM_WINDOW_DATA *)panel_userptr(current_win))->x_pos -= 2; // Console font ratio is 2 to 1
                    break;
                case KEY_RIGHT:
                    ((CWM_WINDOW_DATA *)panel_userptr(current_win))->x_pos += 2;
                    break;
                default:
                    break;
                }

                debug_print("...to (%i, %i)\n", ((CWM_WINDOW_DATA *)panel_userptr(current_win))->x_pos, ((CWM_WINDOW_DATA *)panel_userptr(current_win))->y_pos);

                cwm_window_move(current_win, ((CWM_WINDOW_DATA *)panel_userptr(current_win))->y_pos, ((CWM_WINDOW_DATA *)panel_userptr(current_win))->x_pos);
            }

            if (key == KEY_END) {
                mode = no_mode;
                cwm_window_status(current_win, 0);
            }
        }
        debug_print("\n\n");
    }

    endwin();

    debug_shutdown();
}

/* int main() {
    debug_setup();
    debug_print("Starting...\n");

    // Init
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    debug_print("Finished setting up ncurses.\n");


    //CWM_WINDOW_DATA *win_1 = malloc(sizeof(CWM_WINDOW_DATA));
    //*win_1 = cwm_window_create(10, 100, 0, 0);
    DLL *current_window_in_dll = NULL;
    current_window_in_dll = cwm_windows_new(current_window_in_dll, 10, 100, 0, 0);

    cwm_window_refresh(current_win_data);

    current_window_in_dll = cwm_windows_new(current_window_in_dll, 10, 50, 15, 25);

    getch();

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

        if (mode == no_mode) {
            debug_print("In no mode\n");
            switch (key)
            {
            case 'e':
                mode = edit_mode;
                cwm_window_status(current_win_data, 'e');
                break;
            case 'm':
                mode = move_mode;
                cwm_window_status(current_win_data, 'm');
                break;
            case 'r':
                mode = resize_mode;
                cwm_window_status(current_win_data, 'r');
                break;
            //case 9:
            //    oc = current_win;
            //    current_win = other_win;
            //    other_win = oc;
            //    top_panel(current_win.panel);
            //    debug_print("Status: %c\n", ACS_ULCORNER);
            //    cwm_window_refresh(current_win);
            //    break;
            default:
                break;
            }
        }

        if (mode == resize_mode) {
            switch (key)
                {
                case KEY_UP:
                    current_win_data->height--;
                    break;
                case KEY_DOWN:
                    current_win_data->height++;
                    break;
                case KEY_LEFT:
                    current_win_data->width -= 2; // Console font ratio is 2 to 1
                    break;
                case KEY_RIGHT:
                    current_win_data->width += 2;
                    break;
                default:
                    break;
                }

            WINDOW *old_win = current_win_data->window;
            current_win_data->window = newwin(current_win_data->height, current_win_data->width, current_win_data->y_pos, current_win_data->x_pos);
            replace_panel(current_win_data->panel, current_win_data->window);
            box(current_win_data->window, 0, 0);
            delwin(old_win);
            cwm_window_refresh(current_win_data);
            update_panels();
            doupdate();
            
            if (key == KEY_END) {
                mode = no_mode;
                cwm_window_status(current_win_data, 0);
            }
        }

        if (mode == move_mode) {
            debug_print("In move mode\n");
            if (
                key == KEY_UP
                || key == KEY_DOWN
                || key == KEY_LEFT
                || key == KEY_RIGHT
            ) {

            debug_print("Moving window from (%i, %i)...\n", current_win_data->x_pos, current_win_data->y_pos);

            switch (key)
                {
                case KEY_UP:
                    current_win_data->y_pos--;
                    break;
                case KEY_DOWN:
                    current_win_data->y_pos++;
                    break;
                case KEY_LEFT:
                    current_win_data->x_pos -= 2; // Console font ratio is 2 to 1
                    break;
                case KEY_RIGHT:
                    current_win_data->x_pos += 2;
                    break;
                default:
                    break;
                }

                debug_print("...to (%i, %i)\n", current_win_data->x_pos, current_win_data->y_pos);

                cwm_window_move(current_win_data, current_win_data->y_pos, current_win_data->x_pos);
            }

            if (key == KEY_END) {
                mode = no_mode;
                cwm_window_status(current_win_data, 0);
            }
        }
        debug_print("\n\n");
    }

    getch();

    endwin();
}
 */


/* DLL *cwm_windows_new(DLL *current_window, int height, int width, int y, int x) {
    CWM_WINDOW_DATA *new_window = malloc(sizeof(CWM_WINDOW_DATA));
    *new_window = cwm_window_create(height, width, y, x);

    return dll_append(current_window, new_window);
} */

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

void cwm_window_refresh(PANEL *panel) {
    wmove(panel_window(panel), 1, 1);
    wprintw(panel_window(panel), "Test");
    cwm_window_status(panel, ((CWM_WINDOW_DATA *)panel_userptr(panel))->status);
    update_panels();
    doupdate();
//    wrefresh(cwm_win->window);
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
//    wrefresh(cwm_win->window);
}
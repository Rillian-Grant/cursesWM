#include <ncurses.h>
#include <panel.h>

typedef struct {
    PANEL *panel;
    WINDOW *window;
} CWM_WINDOW;

PANEL *cwm_window_create(int height, int width, int y, int x);
void cwm_window_print(PANEL *pan, char text[]);

int main() {
    // Init
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    // ...
    printw("App....");
    refresh();

    //WINDOW *win = newwin(10,100, 3, 3);
    //box(win, 0, 0);
    //PANEL *pan = new_panel(win);
    //update_panels();
    //doupdate();

    PANEL *pan = cwm_window_create(10, 100, 3, 3);

    getch();

    //WINDOW *win = panel_window(pan);
    //wmove(win, 1, 1);
    //wprintw(win, "Hello World");
    //wrefresh(win);

    cwm_window_print(pan, "Hello World fdjfJDHFADU HUDHUFDSHUF DSUHFD HUHU HUFUD HUHAUFDSHUHUFDHULAHUHUFDUFHUHUADSUHUFDALHUUDHHUFDLLUHAHUL");

    getch();

    move_panel(pan, 3, 12);

    update_panels();
    doupdate();

    getch();

    endwin();
}

PANEL *cwm_window_create(int height, int width, int y, int x) {
    WINDOW *win = newwin(height, width, y, x);
    box(win, 0, 0);
    PANEL *pan = new_panel(win);
    update_panels();
    doupdate();

    return pan;
}

void cwm_window_print(PANEL *pan, char text[]) {
    WINDOW *win = panel_window(pan);
    wmove(win, 1, 1);
    wprintw(win, text);
    wrefresh(win);
}
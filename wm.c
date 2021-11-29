#include <ncurses.h>
#include <panel.h>

WINDOW *create_newwin(int height, int width, int starty, int startx);

int main() {
    // Init
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // ...
    printw("App....");
    refresh();

    // ...
    int height = 10;
        int width = 40;
        int starty = (LINES - height) / 2;  /* Calculating for a center placement */
        int startx = (COLS - width) / 2;
    WINDOW *test_win;
    test_win = create_newwin(height, width, starty, startx);

    getch();

    endwin();
    return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{       WINDOW *local_win;

        local_win = newwin(height, width, starty, startx);
        box(local_win, 0 , 0);          /* 0, 0 gives default characters 
                                         * for the vertical and horizontal
                                         * lines                        */
        wrefresh(local_win);            /* Show that box                */

        return local_win;
}
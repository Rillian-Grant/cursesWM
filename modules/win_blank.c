#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>
#include "../lib/headers/debug.h"
#include "types.h"


extern bool scope = SCOPE_WINDOW;

int handle(PANEL *current_win, int event) {
    if (event == 'n') {
        debug_print("Running new panel");
        

        return 0;
    } else if (event == 'd') {

        return 0;
    } else {
        return 0;
    }
}
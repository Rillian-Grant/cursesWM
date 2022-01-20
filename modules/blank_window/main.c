#include <stdbool.h>
#include <ncurses.h>
#include <stdlib.h>

#include "../types.h"

int handler(int event, void *event_data, void *global_module_data, CWM_WINDOW_DATA *window_data);
extern Module module = {
    .name = "Blank Window",
    .description = "",
    .global=true,
    .handler=handler,
};

typedef struct global_data {
    int counter;
} global_data;

int handler(int event, void *event_data, void *global_module_data, CWM_WINDOW_DATA *window_data) {
    if (global_module_data == NULL) {
        global_module_data = malloc(sizeof(global_data));
        ((global_data *)global_module_data)->counter = 0;
    }
    
    if (event == MODULE_EVENT_START || (event == MODULE_EVENT_KEY_PRESS && *(int *)event_data == ctrl_letter('n'))) return MODULE_RETURN_WIN_NEW;

    if (event == MODULE_EVENT_STOP || (event == MODULE_EVENT_KEY_PRESS && *(int *)event_data == ctrl_letter('d'))) return MODULE_RETURN_WIN_DEL;

    if (event == MODULE_EVENT_WIN_CREATED || event == MODULE_EVENT_WIN_RECREATED) {
        int ret = mvwprintw(window_data->associated_ncurses_window, 1, 1, "Blank window %i", ((global_data *)global_module_data)->counter);
        if (ret == OK) return MODULE_RETURN_DONE;
        else if (ret == ERR) {
            return 99;
        }
    }
    
    return MODULE_RETURN_NOP;
}

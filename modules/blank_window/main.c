#include <stdbool.h>
#include <ncurses.h>
#include <stdlib.h>

#include "../types.h"

int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data);
extern Module module = {
    .name = "Blank Window",
    .description = "",
    .global=true,
    .handler=handler,
};

typedef struct global_data_s {
    int counter;
} global_data_s;

typedef struct window_data_s {
    int number;
} window_data_s;

int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data) {
    if (*global_module_data == NULL) {
        *global_module_data = malloc(sizeof(global_data_s));
        (*(global_data_s **)global_module_data)->counter = 0;
    }
    
    if (event == MODULE_EVENT_START || (event == MODULE_EVENT_KEY_PRESS && *(int *)event_data == ctrl_letter('n'))) return MODULE_RETURN_WIN_NEW;

    if (event == MODULE_EVENT_STOP || (event == MODULE_EVENT_KEY_PRESS && *(int *)event_data == ctrl_letter('d'))) return MODULE_RETURN_WIN_DEL;

    if (event == MODULE_EVENT_WIN_CREATED) {
        int ret = mvwprintw(window_data->associated_ncurses_window, 1, 1, "Blank window %i", (*(global_data_s **)global_module_data)->counter);
        
        *(window_data->module_data) = malloc(sizeof(window_data_s));
        (*(window_data_s **)window_data->module_data)->number = (*(global_data_s **)global_module_data)->counter;

        (*(global_data_s **)global_module_data)->counter++;

        return MODULE_RETURN_DONE;
        /* if (ret == OK) return MODULE_RETURN_DONE
        else if (ret == ERR) {
            return 99;
        } */
    }
    if (event == MODULE_EVENT_WIN_RECREATED) {
        int ret = mvwprintw(window_data->associated_ncurses_window, 1, 1, "Blank window %i", (*(window_data_s **)window_data->module_data)->number);
        return MODULE_RETURN_DONE;
    }
    
    return MODULE_RETURN_NOP;
}

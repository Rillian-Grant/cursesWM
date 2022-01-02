#include <stdbool.h>

#include "../types.h"

int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data);

extern Module info = {
    .name="Window Move",
    .description="",
    .global=true,
    .handler=handler,
};

int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data) {
    if (event != MODULE_EVENT_KEY_PRESS) return MODULE_RETURN_NOP;
    
    int key = *(int *)event_data;

    if ( key == KEY_CTRL_UP) window_data->y_pos--;
    else if ( key == KEY_CTRL_DOWN) window_data->y_pos++;
    else if ( key == KEY_CTRL_LEFT) window_data->x_pos -= 2; // Console font ratio is 2 to 1
    else if ( key == KEY_CTRL_RIGHT) window_data->x_pos += 2;
    
    else return MODULE_RETURN_NOP;

    return MODULE_RETURN_DONE;
}

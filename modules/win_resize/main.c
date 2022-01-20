#include <stdbool.h>
#include "../types.h"

int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data);

extern Module module = {
    .name="Window Resize",
    .description="",
    .global=true,
    .handler=handler,
};

int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data) {
    if (event != MODULE_EVENT_KEY_PRESS) return MODULE_RETURN_NOP;
    
    int key = *(int *)event_data;

    if ( key == KEY_ALT_UP) window_data->height--;
    else if ( key == KEY_ALT_DOWN) window_data->height++;
    else if ( key == KEY_ALT_LEFT) window_data->width -= 2; // Console font ratio is 2 to 1
    else if ( key == KEY_ALT_RIGHT) window_data->width += 2;
    
    else return MODULE_RETURN_NOP;

    return MODULE_RETURN_DONE;
}

#include <stdbool.h>

#include "../types.h"

int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data);
extern Module module = {
    .name = "Blank Window",
    .description = "",
    .global=true,
    .handler=handler,
};

int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data) {
    if (event != MODULE_EVENT_KEY_PRESS && event != MODULE_EVENT_START) return MODULE_RETURN_NOP;

    if (event == MODULE_EVENT_START || (event == MODULE_EVENT_KEY_PRESS && *(int *)event_data == ctrl_letter('n'))) return MODULE_RETURN_WIN_NEW;

    if (event == MODULE_EVENT_STOP || (event == MODULE_EVENT_KEY_PRESS && *(int *)event_data == ctrl_letter('d'))) return MODULE_RETURN_WIN_DEL;

    if (event == MODULE_EVENT_WIN_CREATED) return MODULE_RETURN_NOP;
    
    return MODULE_RETURN_NOP;
}

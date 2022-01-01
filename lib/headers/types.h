#ifndef TYPES_H
#define TYPES_H

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

// ### KEYCODES ###
// TODO Figure out the proper way to do this.

#define KEY_TAB 9

#define ctrl_letter(x) ((x) & 0x1f)

#define KEY_CTRL_UP     566
#define KEY_CTRL_DOWN   525
#define KEY_CTRL_LEFT   545
#define KEY_CTRL_RIGHT  560

#define KEY_ALT_UP      564
#define KEY_ALT_DOWN    523
#define KEY_ALT_LEFT    543
#define KEY_ALT_RIGHT   558

#endif // TYPES_H

// ### Module ###
typedef int (*handler_f)(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data);

// Event
#define MODULE_EVENT_START          0
#define MODULE_EVENT_KEY_PRESS      1
#define MODULE_EVENT_WIN_MINIMIZED  2
#define MODULE_EVENT_WIN_RESTORED   3

// Return
#define MODULE_RETURN_NOP           0
#define MODULE_RETURN_DONE          1
#define MODULE_RETURN_WIN_NEW       2
#define MODULE_RETURN_WIN_DEL       3
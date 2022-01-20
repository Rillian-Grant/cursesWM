/**
 * @file types.h
 * @author Rillian Grant (rillian.grant@gmail.com)
 * @brief Type definitions shared between the core and modules.
 * @version 0.1
 * @date 2022-01-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef TYPES_H
#define TYPES_H

typedef struct Module Module;
typedef struct CWM_WINDOW_DATA CWM_WINDOW_DATA;
typedef int (*handler_f)(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data);

/**
 * @brief Contains all window data.
 * Values preceeded by an underscore are not to be modified by modules.
 */
typedef struct CWM_WINDOW_DATA {
    int x_pos, _x_pos;
    int y_pos, _y_pos;
    int height, _height;
    int width, _width;
    char status, _status; ///< The current status character that appears at the top left corner of the window.
    Module *associated_module; ///< The module that created this window.
    void *associated_ncurses_window; ///< The ncurses window. It is a void pointer so that the modules that don't need to use it don't have to include ncures.h.
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

/**
 * @brief Module information and functions.
 * 
 */
typedef struct Module {
    char *name;         ///< The short name of the module.
    char *description;  ///< The full description of the module.
    bool global;        ///< If true this module is run for all events, not only those effecting it's window(s).
    handler_f handler;  ///< Function pointer to the event handler for the module.
} Module;

// Event
#define MODULE_EVENT_START          0
#define MODULE_EVENT_STOP           1
#define MODULE_EVENT_KEY_PRESS      2
#define MODULE_EVENT_WIN_CREATED    3
#define MODULE_EVENT_WIN_MINIMIZED  4
#define MODULE_EVENT_WIN_RESTORED   5
#define MODULE_EVENT_WIN_RECREATED  6

// Return
#define MODULE_RETURN_NOP           0
#define MODULE_RETURN_DONE          1
#define MODULE_RETURN_WIN_NEW       2
#define MODULE_RETURN_WIN_DEL       3

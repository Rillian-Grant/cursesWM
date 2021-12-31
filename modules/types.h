#ifndef MODULE_TYPES_H
#define MODULE_TYPES_H

#define SCOPE_WINDOW 0
#define SCOPE_EVERYTHING 1

#define RETURN_DONE 0
#define RETURN_PASS 1
#define RETURN_NEW_WIN 3
#define RETURN_DELETE_WIN 4

/**
 * @brief Contains all window data
 * 
 */
typedef struct CWM_WINDOW_DATA {
    int x_pos, _x_pos;                                      /**< The current x coordinate of the window. Is kept up to date. */
    int y_pos, _y_pos;                                      /**< The current y coordinate of the window. Is kept up to date. */
    int height, _height;                                     /**< The current height of the window. Is kept up to date. */
    int width, _width;                                      /**< The current width of the window. Is kept up to date. */
    char status, _status;                                    /**< The current status character of the window. Is shown in the top left hand corner */
} CWM_WINDOW_DATA;

/* typedef struct ModuleEventContext {
    PANEL *current_win;
} ModuleEventContext;

typedef struct ModuleEventReturn {
    PANEL *current_win;
} ModuleEventReturn;

typedef struct ModuleEventBinding {
    int event;          // Values under 1000 or 777 or whatever corrispond to keys.
    struct ModuleEventReturn (*function)(struct ModuleEventContext,  struct ModuleEventBinding *); // Function pointer to run. Need struct 
} ModuleEventBinding; */

#endif
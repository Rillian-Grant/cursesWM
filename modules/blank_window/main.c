#include <stdbool.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"

// Definitions
#define EVENT_CUSTOM_EXIT       (MODULE_MENU_EVENT_OFFSET + 1)
#define EVENT_CUSTOM_INCREMENT  (MODULE_MENU_EVENT_OFFSET + 2)
#define EVENT_CUSTOM_DOUBLE     (MODULE_MENU_EVENT_OFFSET + 3)
#define EVENT_CUSTOM_SQUARE     (MODULE_MENU_EVENT_OFFSET + 4)

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

    if (window_data != NULL && window_data->menu == NULL) {
        MenuItem *blank_window_menu = malloc(sizeof(MenuItem));
        strcpy(blank_window_menu->name, module.name);
        blank_window_menu->event=0;

        MenuItem *increment = malloc(sizeof(MenuItem));
        strcpy(increment->name, "Increment");
        increment->event = EVENT_CUSTOM_INCREMENT;

        MenuItem *exit = malloc(sizeof(MenuItem));
        strcpy(exit->name, "Exit");
        exit->event = EVENT_CUSTOM_EXIT;

        MenuItem *maths_ops_menu = malloc(sizeof(MenuItem));
        strcpy(maths_ops_menu->name, "Advanced maths operations");
        maths_ops_menu->event=0;

        MenuItem *double_num = malloc(sizeof(MenuItem));
        strcpy(double_num->name, "Double");
        double_num->event=EVENT_CUSTOM_DOUBLE;

        MenuItem *square = malloc(sizeof(MenuItem));
        strcpy(square->name, "Square");
        square->event=EVENT_CUSTOM_SQUARE;

        DL_APPEND(window_data->menu, blank_window_menu);
        DL_APPEND(blank_window_menu->submenu, increment);
        DL_APPEND(blank_window_menu->submenu, exit);

        DL_APPEND(window_data->menu, maths_ops_menu);
        DL_APPEND(maths_ops_menu->submenu, double_num);
        DL_APPEND(maths_ops_menu->submenu, square);
    }
    
    if (event == MODULE_EVENT_START || (event == MODULE_EVENT_KEY_PRESS && *(int *)event_data == ctrl_letter('n'))) return MODULE_RETURN_WIN_NEW;

    if (event == MODULE_EVENT_STOP || event == EVENT_CUSTOM_EXIT || (event == MODULE_EVENT_KEY_PRESS && *(int *)event_data == ctrl_letter('d'))) return MODULE_RETURN_WIN_DEL;

    if (event == MODULE_EVENT_WIN_CREATED) {
        int c = (*(global_data_s **)global_module_data)->counter;
        int ret = mvwprintw(window_data->associated_ncurses_window, 1, 1, "Blank window %i", c);
        
        *(window_data->module_data) = malloc(sizeof(window_data_s));
        (*(window_data_s **)window_data->module_data)->number = c;

        window_data->x_pos += 2*c;
        window_data->y_pos += c;

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
    
    if (event == EVENT_CUSTOM_INCREMENT) {
        (*(window_data_s **)window_data->module_data)->number++;
        int ret = mvwprintw(window_data->associated_ncurses_window, 1, 1, "Blank window %i", (*(window_data_s **)window_data->module_data)->number);
        return MODULE_RETURN_DONE;
    }

    if (event == EVENT_CUSTOM_DOUBLE) {
        (*(window_data_s **)window_data->module_data)->number *= 2;
        int ret = mvwprintw(window_data->associated_ncurses_window, 1, 1, "Blank window %i", (*(window_data_s **)window_data->module_data)->number);
        return MODULE_RETURN_DONE;
    }

    if (event == EVENT_CUSTOM_SQUARE) {
        int n = (*(window_data_s **)window_data->module_data)->number;
        (*(window_data_s **)window_data->module_data)->number = n*n;
        int ret = mvwprintw(window_data->associated_ncurses_window, 1, 1, "Blank window %i", (*(window_data_s **)window_data->module_data)->number);
        return MODULE_RETURN_DONE;
    }

    return MODULE_RETURN_NOP;
}

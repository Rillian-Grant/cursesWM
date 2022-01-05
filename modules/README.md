# Module specification

Each module consists of a function called `handler` with the following definition:

`int handler(int event, void *event_data, void **global_module_data, CWM_WINDOW_DATA *window_data)`

- `int event` Contains the reason that this modules handler was called. It can be:
  - `KEY_PRESS`
  - `START` Generic launch command
  - `STOP` Generic stop
  - `WIN_CREATED`
  - `WIN_RECREATED` Happens when the window is resized
  - `WIN_MINIMIZED` Not implemented
  - `WIN_RESTORED` Not implemented

- `void *event_data` Contextual information. Data type depends on the event or NULL.
  - `KEY_PRESS -> char` The key pressed

- `void **global_module_data` Is passed every time the handler is run. The handler is expected to allocate memory etc.

- `CWM_WINDOW_DATA *window_data` See below. Is NULL if the handler was run without an associated window.

- Return values (`int`):
  - `NOP` Did nothing.
  - `DONE` Possibly did something. CWM_WINDOW_DATA is checked and the window is refreshed
  - `WIN_NEW` Rerun with a new blank window.
  - `WIN_DEL` Deletes the window. All user pointers must be deleted before returning this to avoid memory leaks.

## CWM_WINDOW_DATA

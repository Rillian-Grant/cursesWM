#ifndef DLL_H
#define DLL_H

#include <stdlib.h>

typedef struct DLL DLL;
struct DLL {
    void *data;

    DLL *next;
    struct DLL *prev;
};

DLL *dll_seek_tail(DLL *dll);
DLL *dll_seek_head(DLL *dll);

DLL *dll_init(void *data);
DLL *dll_append(DLL *dll, void *data);
DLL *dll_remove(DLL *dll);

#endif
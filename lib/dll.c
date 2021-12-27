#include <stdlib.h>

#include "headers/debug.h"

typedef struct DLL DLL;
struct DLL {
    void *data;

    DLL *next;
    struct DLL *prev;
};

DLL *dll_init(void *data) {
    DLL *node = malloc(sizeof(DLL));

    node->next = NULL;
    node->prev = NULL;

    node->data = data;

    return node;
}

DLL *dll_seek_tail(DLL *dll) {
    debug_print("Seek Tail\n");
    while (dll->next != NULL) {
        dll = dll->next;
    }
    return dll;
}

DLL *dll_seek_head(DLL *dll) {
    while (dll->prev != NULL) {
        dll = dll->prev;
    }
    return dll;
}

DLL *dll_append(DLL *dll, void *data) {
    if (dll == NULL) {
        DLL *node = malloc(sizeof(DLL));

        node->next = NULL;
        node->prev = NULL;

        node->data = data;

        return node;
    } else {
    dll = dll_seek_tail(dll);

    DLL *new_node = malloc(sizeof(DLL));

    new_node->prev = dll;
    new_node->next = NULL;

    return new_node;
    }
}

DLL *dll_remove(DLL *dll) {
    DLL *next = dll->next;
    DLL *prev = dll->prev;

    free(dll);

    prev->next = next;
    next->prev = prev;

    if (next != NULL) return next;
    else if (prev != NULL) return prev;
    else return NULL;
}

/*
Dll *dll_add_below(Dll *dll) {
    Dll *new_node = malloc(sizeof(Dll));
    new_node->next = NULL;
    new_node->prev = dll;
    return new_node;
}

Dll *dll_add_above(Dll *dll) {
    Dll *new_node = malloc(sizeof(Dll));
    new_node->next = dll;
    new_node->prev = NULL;
    return new_node;
}

Dll *remove(Dll *dll) {
    Dll *nxt = dll->next;
    Dll *pre = dll->prev;
    pre->next = nxt;
    nxt->prev = pre;

    free(dll);

    if (nxt != NULL) return nxt;
    if (pre != NULL) return pre;
    else return NULL;
}
*/
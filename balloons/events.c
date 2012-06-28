#include "events.h"

static events *ev_make(void) {
    events *e = malloc(sizeof(events));
    zero(e, sizeof(events));
    return e;
}

static void ev_keyset(events *e, char *k) {
    e->name = malloc(strlen(k) + 1);
    strcpy(e->name, k);
}

events *ev_get_global() {
    static events *e = NULL;
    if (e == NULL) e = ev_make();
    return e;
}

void ev_hook(events *e, char *evname, damn_callback d) {
    if (e->name == NULL) {
        ev_keyset(e, evname);
        e->d = d;
        return;
    }
    
    while (e->next != NULL)
        e = e->next;
    
    e->next = ev_make();
    ev_keyset(e->next, evname);
    e->next->d = d;
}

void ev_trigger(events *e, char *evname, damn *d, packet *p) {
    do {
        if (strcmp(e->name, evname) == 0)
            e->d(d, p);
    } while ((e = e->next) != NULL);
}

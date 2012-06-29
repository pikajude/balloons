#include "events.h"

static void ev_keyset(events *e, char *k) {
    e->name = calloc(1, strlen(k) + 1);
    strcpy(e->name, k);
}

events *ev_make(void) {
    events *e = calloc(1, sizeof(events));
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

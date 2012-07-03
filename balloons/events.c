#include "events.h"

static unsigned long event_counter = 0;

static void ev_keyset(events *e, char *k) {
    e->name = calloc(1, strlen(k) + 1);
    strcpy(e->name, k);
}

events *ev_make(void) {
    events *e = calloc(1, sizeof(events));
    e->id = event_counter++;
    return e;
}

unsigned long ev_hook(events *e, char *evname, damn_callback d) {
    if (e->name == NULL) {
        ev_keyset(e, evname);
        e->d = d;
        return e->id;
    }
    
    while (e->next != NULL)
        e = e->next;
    
    e->next = ev_make();
    ev_keyset(e->next, evname);
    e->next->d = d;
    return e->next->id;
}

void ev_unhook(events *e, unsigned long id) {
    events *cur = e;
    if (e == NULL) return;
    for (;;) {
        if (cur->next != NULL && cur->next->id == id) {
            events *nextnext = cur->next->next;
            free(cur->next->name);
            free(cur->next);
            cur->next = nextnext;
        }
        cur = cur->next;
        if (cur == NULL) return;
    }
}

void ev_trigger(events *e, char *evname, damn *d, packet *p) {
    events *cur = e;
    do {
        if (strcmp(cur->name, evname) == 0)
            cur->d(e, d, p);
    } while ((cur = cur->next) != NULL);
}

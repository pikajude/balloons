#include "events.h"

static unsigned long event_counter = 0;
static events *evtglob = NULL;

static void ev_keyset(events *e, char *k) {
    e->name = calloc(1, strlen(k) + 1);
    if (e->name == NULL)
        HANDLE_ERR("Unable to allocate space for e->name");
    strcpy(e->name, k);
}

static events *ev_make(void) {
    events *e = calloc(1, sizeof *e);
    if (e == NULL)
        HANDLE_ERR("Unable to allocate an events");
    e->id = event_counter++;
    e->access = 0;
    return e;
}

events *ev_get_global(void) {
    if (evtglob == NULL)
        evtglob = ev_make();
    return evtglob;
}

unsigned long ev_hook(char *evname, damn_callback d, unsigned char _access, bool nothread) {
    events *e = ev_get_global();
    if (e->name == NULL) {
        ev_keyset(e, evname);
        e->d = d;
        e->access = _access;
        e->threaded = !nothread;
        return e->id;
    }
    
    while (e->next != NULL)
        e = e->next;
    
    e->next = ev_make();
    ev_keyset(e->next, evname);
    e->next->d = d;
    e->next->access = _access;
    e->next->threaded = !nothread;
    return e->next->id;
}

void ev_unhook(unsigned long id) {
    events *cur = ev_get_global();
    if (cur->id == id) {
        evtglob = cur->next;
        free(cur->name);
        free(cur);
        return;
    }
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

void ev_trigger_priv(char *evname, context cbdata, unsigned char level) {
    events *cur = ev_get_global();
    do {
        if (cur->name == NULL)
            return;
        if (strcmp(cur->name, evname) == 0 && cur->access <= level) {
            if (cur->threaded)
                dispatch(cur->name, CMD_TIMEOUT, cur->d, &cbdata);
            else
                cur->d(&cbdata);
        }
    } while ((cur = cur->next) != NULL);
}

void ev_trigger(char *evname, context cbdata) {
    return ev_trigger_priv(evname, cbdata, 255);
}

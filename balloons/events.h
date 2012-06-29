#pragma once

#include <stdlib.h>
#include "damn.h"
#include "packet.h"

typedef void (*damn_callback)(damn*, packet*);

struct _events {
    char *name;
    damn_callback d;
    struct _events *next;
};

typedef struct _events events;

events *ev_make(void);
void ev_hook(events*, char*, damn_callback);
void ev_trigger(events*, char*, damn*, packet*);

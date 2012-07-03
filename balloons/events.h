#pragma once

#include <stdlib.h>
#include "damn.h"
#include "packet.h"

struct _events;

struct _callback_data {
    struct _events *evt;
    damn *damn;
    packet *pkt;
    char *msg;
};

typedef struct _callback_data callback_data;

typedef void (*damn_callback)(callback_data);

struct _events {
    unsigned long id;
    char *name;
    damn_callback d;
    struct _events *next;
};

typedef struct _events events;

events *ev_make(void);
unsigned long ev_hook(events*, char*, damn_callback);
void ev_unhook(events*, unsigned long);
void ev_trigger(events*, char*, callback_data);

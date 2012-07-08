#pragma once

#include <stdlib.h>
#include "damn.h"
#include "packet.h"

struct _events;

typedef struct {
    damn *damn;
    packet *pkt;
    char *msg;
    char *sender;
} context;

typedef void (*damn_callback)(context);

struct _events {
    unsigned long id;
    char *name;
    damn_callback d;
    struct _events *next;
};

typedef struct _events events;

events *ev_get_global(void);
unsigned long ev_hook(char*, damn_callback);
void ev_unhook(unsigned long);
void ev_trigger(char*, context);

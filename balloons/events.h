#pragma once

#include <stdlib.h>

struct _damn;

#include "damn/damn.h"
#include "packet/packet.h"
#include "timed.h"

struct _events;

struct _context {
    struct _damn *damn;
    packet *pkt;
    char *msg;
    char *sender;
};

typedef struct _context context;

typedef void (*damn_callback)(context *);

struct _events {
    unsigned long id;
    char *name;
    damn_callback d;
    unsigned char access;
    bool async;
    struct _events *next;
};

typedef struct _events events;

events *ev_get_global(void);
unsigned long ev_hook(char*, damn_callback, unsigned char, bool);
void ev_unhook(unsigned long);
void ev_trigger_priv(char*, context, unsigned char);
void ev_trigger(char*, context);

#define ev_hookany(a,b) ev_hook((a),(b),0,true)

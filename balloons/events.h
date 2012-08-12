#pragma once

#include <stdlib.h>

struct _damn;

#include "damn.h"
#include "packet.h"
#include "timed.h"

struct _events;

struct _context {
    struct _damn *damn;
    packet *pkt;
    wchar_t *msg;
    wchar_t *sender;
};

typedef struct _context context;

typedef void (*damn_callback)(context *);

struct _events {
    unsigned long id;
    wchar_t *name;
    damn_callback d;
    unsigned char access;
    bool async;
    unsigned int timeout;
    struct _events *next;
};

typedef struct _events events;

events *ev_get_global(void);
unsigned long ev_hook(wchar_t*, damn_callback, unsigned char, bool, unsigned int);
void ev_unhook(unsigned long);
void ev_trigger_priv(wchar_t*, context, unsigned char);
void ev_trigger(wchar_t*, context);

#define ev_hookany(a,b) ev_hook((a),(b),0,false,0)

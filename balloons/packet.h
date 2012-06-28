#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alist.h"
#include "defs.h"

struct _packet {
    char *command;
    char *subcommand;
    arglist *args;
    char *body;
};

typedef struct _packet packet;

packet *packet_parse(const char*, bool);
void packet_free(packet*);
void packet_print(packet*);

#define subpacket(p) packet_parse(p->body, false)
#define parse(str) packet_parse(str, strncmp(str, "login ", 6) == 0)
#define parg_get(p,arg) al_get(p->args, arg)

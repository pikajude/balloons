#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alist.h"
#include "defs.h"

typedef struct {
    char *command;
    char *subcommand;
    arglist *args;
    char *body;
} packet;

packet *packet_parse(const char*, int);
void pkt_free(packet*);
void pkt_print(packet*);

#define pkt_subpacket(p) packet_parse(p->body, false)
#define pkt_parse(str) packet_parse(str, strncmp(str, "login ", 6) == 0)
#define pkt_getarg(p,arg) al_get(p->args, arg)
#define pkt_roomname(p) (p->subcommand + 5)
#define pkt_from(p) al_get(p->args, "from")

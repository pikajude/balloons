#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alist.h"
#include "defs.h"

typedef struct {
    wchar_t *command;
    wchar_t *subcommand;
    arglist *args;
    wchar_t *body;
    int ref;
} packet;

packet *packet_parse(const wchar_t*, int);
void pkt_free(packet*);
void pkt_print(packet*);

#define pkt_subpacket(p) packet_parse(p->body, false)
#define pkt_parse(str) packet_parse(str, strncmp((char *)(str), "login ", 6) == 0 || strncmp((char *)(str), "property login", 14) == 0)
#define pkt_getarg(p,arg) al_get(p->args, arg)
#define pkt_roomname(p) (p->subcommand + 5)
#define pkt_from(p) al_get(p->args, L"from")

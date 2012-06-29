#include <stdio.h>
#include "api.h"
#include "damn.h"
#include "packet.h"
#include "token.h"
#include "protocol.h"
#include "events.h"
#include "handlers.h"

static void handhandler(damn *d, packet *p) {
    printf("connected! reconnect wait: %dms\n", d->reconnect_wait);
}

static void getevtname(char *name, packet *p) {
    strcat(name, p->command);
    if (strcmp(p->command, "property") == 0) {
        strcat(name, ".");
        strcat(name, parg_get(p, "p"));
    } else if (strcmp(p->command, "recv") == 0) {
        size_t loc = 9, strloc = 0;
        strcat(name, ".");
        while (p->body[strloc] != ' ')
            name[loc++] = p->body[strloc++];
    }
}

int main (int argc, const char *argv[])
{
    char *pkt;
    packet *p;
    char evtid[25] = "pkt.";
    
    events *e = ev_make();
    load_libs(e);
    ev_hook(e, "pkt.dAmnServer", &handler_dAmnServer);
    ev_hook(e, "pkt.login", &handler_login);
    ev_hook(e, "pkt.property.members", &handler_property_members);
    
    char *tok = token_get_access_all();
    set_damntoken(token_get_damn(tok));
    
    damn *d = damn_make(true);
    phandshake(d);
    
    for (;;) {
        pkt = damn_read(d);
        p = parse(pkt);
        
        getevtname(evtid, p);
        ev_trigger(e, evtid, d, p);
        exec_commands(e, d, p);
        
        packet_free(p);
        free(pkt);
        zero(evtid + 4, 18);
    }

    return 0;
}

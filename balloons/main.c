#include <stdio.h>
#include "api.h"
#include "damn.h"
#include "packet.h"
#include "token.h"
#include "protocol.h"
#include "events.h"
#include "handlers.h"
#include "setup.h"

static void getevtname(char *name, packet *p) {
    strcat(name, "pkt.");
    strcat(name, p->command);
    if (strcmp(p->command, "property") == 0) {
        strcat(name, ".");
        strcat(name, pkt_getarg(p, "p"));
    } else if (strcmp(p->command, "recv") == 0) {
        size_t loc = 9, strloc = 0;
        strcat(name, ".");
        while (p->body[strloc] != ' ')
            name[loc++] = p->body[strloc++];
    }
}

int main (int argc, const char *argv[])
{
    runsetup();
    
    char *pkt;
    packet *p;
    char *evtid = calloc(1, 25);
    
    printf("%p\n", hook_msg);
    
    ev_hook("pkt.dAmnServer", &handler_dAmnServer);
    load_libs();
    ev_hook("pkt.login", &handler_login);
    ev_hook("pkt.ping", &handler_ping);
    ev_hook("pkt.property.members", &handler_property_members);
    
    char *tok = token_get_access_all();
    set_damntoken(token_get_damn(tok));
    
    damn *d = damn_make();
    dhandshake(d);
    
    for (;;) {
        pkt = damn_read(d);
        if (pkt == NULL) {
            printf("Whoops, disconnected.\n");
            damn_disconnect(d);
            d = damn_make();
            dhandshake(d);
            pkt = damn_read(d);
        }
        p = pkt_parse(pkt);
        
        getevtname(evtid, p);
        ev_trigger(evtid, (event_data){ d, p, p->body });
        exec_commands(d, p);
        
        pkt_free(p);
        free(pkt);
        zero(evtid, 25);
    }

    return 0;
}

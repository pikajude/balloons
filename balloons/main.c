#include <stdio.h>
#include "api.h"
#include "damn.h"
#include "packet.h"
#include "token.h"
#include "protocol.h"
#include "events.h"
#include "handlers.h"
#include "setup.h"
#include "timed.h"

static void getevtname(char *name, packet *p) {
    strcat(name, "pkt.");
    strcat(name, p->command);
    if (strcmp(p->command, "property") == 0) {
        strcat(name, ".");
        strcat(name, pkt_getarg(p, "p"));
    } else if (strcmp(p->command, "recv") == 0) {
        assert(p->body != NULL);
        size_t loc = 9, strloc = 0;
        strcat(name, ".");
        while (p->body[strloc] != ' ')
            name[loc++] = p->body[strloc++];
    }
}

int main (int argc, const char *argv[])
{
    signal(SIGCHLD, SIG_IGN); // prevent exiting children from killing process
    runsetup();
    
    char *pkt;
    packet *p;
    char *evtid = calloc(1, 25);
    if (evtid == NULL)
        HANDLE_ERR("Unable to allocate for event ID");
    
    ev_hookany("pkt.dAmnServer", &handler_dAmnServer);
    load_libs();
    ev_hookany("pkt.login", &handler_login);
    ev_hookany("pkt.ping", &handler_ping);
    ev_hookany("pkt.property.members", &handler_property_members);
    ev_hookany("pkt.property.topic", &handler_property_topic);
    ev_hookany("pkt.property.title", &handler_property_title);
    ev_hookany("pkt.property.privclasses", &handler_property_privclasses);
    ev_hookany("pkt.recv.msg", &handler_recv_msg);

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
        if(p->body != NULL)
            p->body = delump(p->body);
        
        getevtname(evtid, p);
        ev_trigger(evtid, (context){ d, p, p->body, NULL });
        exec_commands(d, p);
        
        pkt_free(p);
        free(pkt);
        zero(evtid, 25);
    }

    return 0;
}

#include <stdio.h>
#include <locale.h>
#include "api.h"
#include "damn.h"
#include "packet.h"
#include "token.h"
#include "protocol.h"
#include "events.h"
#include "handlers.h"
#include "setup.h"
#include "timed.h"
#include "htmlentities.h"

static void getevtname(wchar_t *name, packet *p) {
    swprintf(name, wcslen(p->command) + 5, L"pkt.%ls", p->command);
    if (wcscmp(p->command, L"property") == 0) {
        wcscat(name, L".");
        wcscat(name, pkt_getarg(p, L"p"));
    } else if (wcscmp(p->command, L"recv") == 0) {
        assert(p->body != NULL);
        size_t loc = 9, strloc = 0;
        wcscat(name, L".");
        while (p->body[strloc] != ' ')
            name[loc++] = btowc(p->body[strloc++]);
    }
}

int main (int argc, const char *argv[])
{
    setlocale(LC_ALL, "");
    
    signal(SIGCHLD, SIG_IGN); // prevent exiting children from killing process
    runsetup();
    
    char *pkt;
    packet *p;
    wchar_t *evtid = calloc(1, sizeof(wchar_t) * 25);
    if (evtid == NULL)
        HANDLE_ERR("Unable to allocate for event ID");
    
    ev_hookany(L"pkt.dAmnServer", &handler_dAmnServer);
    load_libs();
    ev_hookany(L"pkt.login", &handler_login);
    ev_hookany(L"pkt.ping", &handler_ping);
    ev_hookany(L"pkt.join", &handler_join);
    ev_hookany(L"pkt.property.members", &handler_property_members);
    ev_hookany(L"pkt.property.topic", &handler_property_topic);
    ev_hookany(L"pkt.property.title", &handler_property_title);
    ev_hookany(L"pkt.property.privclasses", &handler_property_privclasses);
    ev_hookany(L"pkt.recv.msg", &handler_recv_msg);
    ev_hookany(L"pkt.recv.action", &handler_recv_action);
    ev_hookany(L"pkt.recv.join", &handler_recv_join);
    ev_hookany(L"pkt.recv.part", &handler_recv_part);

    wchar_t *tok = token_get_access_all();
    set_damntoken(token_get_damn(tok));
    
    damn *d = damn_make();
    dhandshake(d);
    
    context *c;
    
    for (;;) {
        pkt = damn_read(d);
        if (pkt == NULL) {
            printf("Whoops, disconnected.\n");
            damn_disconnect(d);
            d = damn_make();
            dhandshake(d);
            pkt = damn_read(d);
        }
        wchar_t *pktd = entity_decode(pkt);
        free(pkt);
        p = pkt_parse(pktd);
        if(p->body != NULL)
            p->body = delump(p->body);
        
        p->ref = 0;
        
        getevtname(evtid, p);
        c = malloc(sizeof(context));
        c->damn = d;
        c->pkt = p;
        c->msg = p->body;
        c->sender = NULL;
        ev_trigger(evtid, c);
        exec_commands(d, p);
        
        if(p->ref == 0)
            free(p);
        wmemset(evtid, 0, 25);
    }

    return 0;
}

#include <stdio.h>
#include "damn.h"
#include "packet.h"
#include "token.h"
#include "protocol.h"
#include "events.h"

static void handhandler(damn *d, packet *p) {
    printf("connected! reconnect wait: %dms\n", d->reconnect_wait);
}

static void handler(damn *d, packet *p) {
    printf("Handshake succeeded.\n");
    char *tok = token_get_access_all();
    char *dtk = token_get_damn(tok);
    login(d, setting_get("_username"), dtk);
}

static void loghandler(damn *d, packet *p) {
    if (strcmp(parg_get(p, "e"), "ok") == 0) {
        printf("Logged in as %s.\n", p->subcommand);
    } else {
        printf("Failed to log in, how the fuck did that happen?\n");
    }
}

int main (int argc, const char *argv[])
{
    char *pkt;
    packet *p;
    char evtid[15] = "pkt.";
    
    events *e = ev_get_global();
    ev_hook(e, "pkt.dAmnServer", &handler);
    ev_hook(e, "pkt.login", &loghandler);
    
    damn *d = damn_make(true);
    handshake(d);
    
    for (;;) {
        pkt = damn_read(d);
        p = parse(pkt);
        
        strcat(evtid, p->command);
        ev_trigger(e, evtid, d, p);
        
        packet_free(p);
        free(pkt);
        zero(evtid + 4, 8);
    }

    return 0;
}

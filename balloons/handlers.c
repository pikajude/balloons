#include "handlers.h"

static char *dtk;

void set_damntoken(char *d) {
    dtk = d;
}

HANDLER(dAmnServer) {
    printf("Handshake succeeded.\n");
    dlogin(cbdata.damn, setting_get(BKEY_USERNAME), dtk);
}

HANDLER(login) {
    if (strcmp(pkt_getarg(cbdata.pkt, "e"), "ok") == 0) {
        printf("Logged in as %s.\n", cbdata.pkt->subcommand);
        djoin(cbdata.damn, "DevelopingDevelopers");
    } else {
        printf("Failed to log in, how the fuck did that happen?\n");
    }
}

HANDLER(ping) {
    dpong(cbdata.damn);
}

HANDLER(property_members) {
    printf("Got members for #%s.\n", pkt_roomname(cbdata.pkt));
}

HANDLER(property_topic) {
    printf("Got topic for #%s.\n", pkt_roomname(cbdata.pkt));
}

HANDLER(property_title) {
    printf("Got title for #%s.\n", pkt_roomname(cbdata.pkt));
}

HANDLER(property_privclasses) {
    printf("Got privclasses for #%s.\n", pkt_roomname(cbdata.pkt));
}

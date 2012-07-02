#include "handlers.h"

static char *dtk;

void set_damntoken(char *d) {
    dtk = d;
}

HANDLER(dAmnServer) {
    printf("Handshake succeeded.\n");
    dlogin(d, setting_get(BKEY_USERNAME), dtk);
}

HANDLER(login) {
    if (strcmp(pkt_getarg(p, "e"), "ok") == 0) {
        printf("Logged in as %s.\n", p->subcommand);
        djoin(d, "DevelopingDevelopers");
    } else {
        printf("Failed to log in, how the fuck did that happen?\n");
    }
}

HANDLER(property_members) {
    printf("Got members for %s\n", p->subcommand);
}

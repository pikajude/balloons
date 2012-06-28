#include "handlers.h"

static char *dtk;

void set_damntoken(char *d) {
    dtk = d;
}

HANDLER(dAmnServer) {
    printf("Handshake succeeded.\n");
    login(d, setting_get("_username"), dtk);
}

HANDLER(login) {
    if (strcmp(parg_get(p, "e"), "ok") == 0) {
        printf("Logged in as %s.\n", p->subcommand);
        join(d, "DevelopingDevelopers");
    } else {
        printf("Failed to log in, how the fuck did that happen?\n");
    }
}

HANDLER(property_members) {
    printf("Got members for %s\n", p->subcommand);
}

HANDLER(recv_msg) {
    printf("%s\n", p->body);
}

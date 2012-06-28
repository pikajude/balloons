#include "handlers.h"

static char *dtk;

void set_damntoken(char *d) {
    dtk = d;
}

HANDLER(dAmnServer) {
    printf("Handshake succeeded.\n");
    plogin(d, setting_get("_username"), dtk);
}

HANDLER(login) {
    if (strcmp(parg_get(p, "e"), "ok") == 0) {
        printf("Logged in as %s.\n", p->subcommand);
        pjoin(d, "DevelopingDevelopers");
    } else {
        printf("Failed to log in, how the fuck did that happen?\n");
    }
}

HANDLER(property_members) {
    printf("Got members for %s\n", p->subcommand);
}

HANDLER(recv_msg) {
    packet *sub = subpacket(p);
    char *us = setting_get("_username");
    char response[50];
    sprintf(response, "Hi, %s!", parg_get(sub, "from"));
    if (strncmp(us, sub->body, strlen(us)) == 0)
        psendmsg(d, p->subcommand + 5, response);
}

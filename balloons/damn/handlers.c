#include "handlers.h"

static char *dtk;

void set_damntoken(char *d) {
    dtk = d;
}

HANDLER(dAmnServer) {
    printf("Handshake succeeded.\n");
    dlogin(cbdata->damn, setting_get(BKEY_USERNAME), dtk);
}

HANDLER(login) {
    if (strcmp(pkt_getarg(cbdata->pkt, "e"), "ok") == 0) {
        printf("Logged in as %s.\n", cbdata->pkt->subcommand);
        djoin(cbdata->damn, "DevelopingDevelopers");
    } else {
        printf("Failed to log in, how the fuck did that happen?\n");
    }
}

HANDLER(ping) {
    dpong(cbdata->damn);
}

HANDLER(property_members) {
    logger(blue, "#");
    logger(blue, pkt_roomname(cbdata->pkt));
    printf(" Got members.\n");
}

HANDLER(property_topic) {
    logger(blue, "#");
    logger(blue, pkt_roomname(cbdata->pkt));
    printf(" Topic: %s\n", delump(cbdata->pkt->body));
}

HANDLER(property_title) {
    logger(blue, "#");
    logger(blue, pkt_roomname(cbdata->pkt));
    printf(" Title: %s\n", delump(cbdata->pkt->body));
}

HANDLER(property_privclasses) {
    logger(blue, "#");
    logger(blue, pkt_roomname(cbdata->pkt));
    printf(" Got privclasses.\n");
}

HANDLER(recv_msg) {
    printf("%s\n", cbdata->pkt->body);
}

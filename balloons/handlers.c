#include "handlers.h"

static wchar_t *dtk;

void set_damntoken(wchar_t *d) {
    dtk = d;
}

HANDLER(dAmnServer) {
    printf("Handshake succeeded.\n");
    dlogin(cbdata->damn, setting_get(BKEY_USERNAME), dtk);
}

HANDLER(login) {
    if (wcscmp(pkt_getarg(cbdata->pkt, L"e"), L"ok") == 0) {
        wprintf(L"Logged in as %ls.\n", cbdata->pkt->subcommand);
        djoin(cbdata->damn, L"DevelopingDevelopers");
    } else {
        printf("Failed to log in, how the fuck did that happen?\n");
    }
}

HANDLER(ping) {
    dpong(cbdata->damn);
}

HANDLER(property_members) {
    logger(blue, L"#%ls", pkt_roomname(cbdata->pkt));
    printf(" Got members.\n");
}

HANDLER(property_topic) {
    logger(blue, L"#%ls", pkt_roomname(cbdata->pkt));
    wprintf(L" Topic: %ls\n", cbdata->pkt->body);
}

HANDLER(property_title) {
    logger(blue, L"#%ls", pkt_roomname(cbdata->pkt));
    wprintf(L" Title: %ls\n", cbdata->pkt->body);
}

HANDLER(property_privclasses) {
    logger(blue, L"#%ls", pkt_roomname(cbdata->pkt));
    printf(" Got privclasses.\n");
}

HANDLER(recv_msg) {
    logger(blue, L"#%ls ", pkt_roomname(cbdata->pkt));
    packet *s = pkt_subpacket(cbdata->pkt);
    logger(green, L"<%ls> ", pkt_from(s));
    wprintf(L"%ls\n", s->body);
}

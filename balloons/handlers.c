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
        wchar_t *split, *room, *autojoin = setting_get(BKEY_AUTOJOIN);
        room = wcstok(autojoin, L" ", &split);
        do {
            djoin(cbdata->damn, room[0] == L'#' ? room + 1 : room);
        } while ((room = wcstok(NULL, L" ", &split)) != NULL);
    } else {
        printf("Failed to log in, how the fuck did that happen?\n");
    }
}

HANDLER(ping) {
    dpong(cbdata->damn);
}

HANDLER(join) {
    if(wcscmp(pkt_getarg(cbdata->pkt, L"e"), L"ok") == 0) {
        logger(blue, L"#%ls", pkt_roomname(cbdata->pkt));
        printf(" Joined.\n");
        add_room(pkt_roomname(cbdata->pkt));
    } else {
        printf("Couldn't join whatever.\n");
    }
}

HANDLER(part) {
    if(wcscmp(pkt_getarg(cbdata->pkt, L"e"), L"ok") == 0) {
        logger(blue, L"#%ls", pkt_roomname(cbdata->pkt));
        printf(" Parted.\n");
        delete_room(pkt_roomname(cbdata->pkt));
    } else {
        printf("Couldn't part whatever.\n");
    }
}

HANDLER(property_members) {
    radd_users(pkt_roomname(cbdata->pkt), cbdata->pkt);
    logger(blue, L"#%ls", pkt_roomname(cbdata->pkt));
    printf(" Got members.\n");
}

HANDLER(property_topic) {
    rset_topic(pkt_roomname(cbdata->pkt), cbdata->pkt->body);
    logger(blue, L"#%ls", pkt_roomname(cbdata->pkt));
    wprintf(L" Topic: %ls\n", cbdata->pkt->body);
}

HANDLER(property_title) {
    rset_title(pkt_roomname(cbdata->pkt), cbdata->pkt->body);
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
    free(s);
}

HANDLER(recv_action) {
    logger(blue, L"#%ls ", pkt_roomname(cbdata->pkt));
    packet *s = pkt_subpacket(cbdata->pkt);
    logger(green, L"* %ls ", pkt_from(s));
    wprintf(L"%ls\n", s->body);
    free(s);
}

HANDLER(recv_join) {
    logger(blue, L"#%ls ", pkt_roomname(cbdata->pkt));
    packet *s = pkt_subpacket(cbdata->pkt);
    logger(green, L"» %ls\n", s->subcommand);
    radd_user(pkt_roomname(cbdata->pkt), s->subcommand);
    free(s);
}

HANDLER(recv_part) {
    logger(blue, L"#%ls ", pkt_roomname(cbdata->pkt));
    packet *s = pkt_subpacket(cbdata->pkt);
    logger(red, L"« %ls\n", s->subcommand);
    rdel_user(pkt_roomname(cbdata->pkt), s->subcommand);
    free(s);
}

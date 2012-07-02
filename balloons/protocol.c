#include "protocol.h"

void dhandshake(damn *d) {
    int sock = d->_sockd;
    dprintf(sock, "dAmnClient 0.3\nagent=balloons %s", BVERSION);
    finish(sock);
}

void dlogin(damn *d, char *user, char *token) {
    int sock = d->_sockd;
    dprintf(sock, "login %s\npk=%s", user, token);
    finish(sock);
}

void djoin(damn *d, char *room) {
    int sock = d->_sockd;
    dprintf(sock, "join chat:%s", room);
    finish(sock);
}

void dpong(damn *d) {
    int sock = d->_sockd;
    dprintf(sock, "pong");
    finish(sock);
}

void dsendmsgtype(damn *d, char *type, char *room, char *msg, ...) {
    va_list args;
    int sock = d->_sockd;
    va_start(args, msg);
    dprintf(sock, "send chat:%s\n\n%s main\n\n", room, type);
    vdprintf(sock, msg, args);
    va_end(args);
    send(sock, "", 1, 0);
}

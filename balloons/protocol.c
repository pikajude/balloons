#include "protocol.h"

void handshake(damn *d) {
    int sock = d->_sockd;
    writestr(sock, "dAmnClient 0.3\nagent=balloons ");
    writestr(sock, BALLOONS_VERSION);
    finish(sock);
}

void login(damn *d, char *user, char *token) {
    int sock = d->_sockd;
    writestr(sock, "login ");
    writestr(sock, user);
    writestr(sock, "\npk=");
    writestr(sock, token);
    finish(sock);
}

void join(damn *d, char *room) {
    int sock = d->_sockd;
    writestr(sock, "join chat:");
    writestr(sock, room);
    finish(sock);
}

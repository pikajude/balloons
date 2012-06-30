#include "protocol.h"

void phandshake(damn *d) {
    int sock = d->_sockd;
    writestr(sock, "dAmnClient 0.3\nagent=balloons ");
    writestr(sock, BVERSION);
    finish(sock);
}

void plogin(damn *d, char *user, char *token) {
    int sock = d->_sockd;
    writestr(sock, "login ");
    writestr(sock, user);
    writestr(sock, "\npk=");
    writestr(sock, token);
    finish(sock);
}

void pjoin(damn *d, char *room) {
    int sock = d->_sockd;
    writestr(sock, "join chat:");
    writestr(sock, room);
    finish(sock);
}

void psendmsg(damn *d, char *room, char *msg) {
    int sock = d->_sockd;
    writestr(sock, "send chat:");
    writestr(sock, room);
    writestr(sock, "\n\nmsg main\n\n");
    writestr(sock, msg);
    send(sock, "", 1, 0);
}

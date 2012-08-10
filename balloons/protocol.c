#include "protocol.h"

static void dsend(damn *d, wchar_t *msg, ...) {
    setlocale(LC_ALL, "");
    va_list args;
    wchar_t *target = calloc(1, sizeof(wchar_t) * 8024);
    va_start(args, msg);
    assert(vswprintf(target, 8024, msg, args) >= 0);
    va_end(args);
    char *towrite = calloc(1, 8024);
    wcstombs(towrite, target, 8024);
    free(target);
    dprintf(d->_sockd, "%s", towrite);
    free(towrite);
}

void dhandshake(damn *d) {
    dsend(d, L"dAmnClient 0.3\nagent=balloons %ls", BVERSION);
    finish(d->_sockd);
}

void dlogin(damn *d, wchar_t *user, wchar_t *token) {
    dsend(d, L"login %ls\npk=%ls", user, token);
    finish(d->_sockd);
}

void djoin(damn *d, wchar_t *room) {
    dsend(d, L"join chat:%ls", room);
    finish(d->_sockd);
}

void dpong(damn *d) {
    int sock = d->_sockd;
    dprintf(sock, "pong");
    finish(sock);
}

void dsendmsgtype(damn *d, char *type, wchar_t *room, wchar_t *msg, ...) {
    va_list args;
    int sock = d->_sockd;
    char *aroom = calloc(1, wcslen(room) * 4);
    wcstombs(aroom, room, wcslen(room) * 4);
    wchar_t *target = calloc(1, 8024 * sizeof(wchar_t));
    char *towrite = calloc(1, 8024);
    va_start(args, msg);
    vswprintf(target, 8024, msg, args);
    va_end(args);
    wcstombs(towrite, target, 8024);
    dprintf(sock, "send chat:%s\n\n%s main\n\n%s", aroom, type, towrite);
    send(sock, "", 1, 0);
    free(towrite);
    free(aroom);
    free(target);
}

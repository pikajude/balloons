#include "damn.h"
#include "events.h"

damn *damn_make(int autoconnect) {
    damn *d = calloc(1, sizeof(damn));
    if (d == NULL)
        return NULL;
    d->reconnect_wait = 5000; // ms
    d->autoreconnect = false;
    d->_sockd = 0;
    if (autoconnect)
        damn_connect(d);
    return d;
}

int damn_connect(damn *d) {
    struct addrinfo addr, *res;
    zero(&addr, sizeof(addr));
    addr.ai_family = AF_INET;
    addr.ai_socktype = SOCK_STREAM;
    getaddrinfo("chat.deviantart.com", "3900", &addr, &res);
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    d->_sockd = sock;
    if(connect(sock, res->ai_addr, res->ai_addrlen) == 0) {
        d->_connected = true;
        return true;
    } else return false;
}

void damn_disconnect(damn *d) {
    close(d->_sockd);
    d->_connected = false;
}

ssize_t damn_write(damn *d, char *pkt) {
    return send(d->_sockd, pkt, strlen(pkt)+1, 0);
}

char *damn_read(damn *d) {
    if (!d->_connected)
        return NULL;
    char *res = calloc(1, 8024);
    char buf[1];
    int loc = 0;
    for(;;) {
        if(recv(d->_sockd, &buf, 1, 0) <= 0) {
            damn_disconnect(d);
            return NULL;
        } else if(*buf == 0) {
            return res;
        } else
            res[loc++] = *buf;
    }
}

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef __WIN32
    #include <winsock.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "defs.h"
#include "events.h"

struct _damn {
    int _sockd;
    bool _connected;
    int autoreconnect;
    int reconnect_wait;
};

typedef struct _damn damn;

damn *damn_make(void);
bool damn_connect(damn*);
void damn_disconnect(damn*);
char* damn_read(damn*);
ssize_t damn_write(damn*, char*);
void damn_finish(damn*);

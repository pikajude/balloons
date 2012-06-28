#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "defs.h"

struct _damn {
    int _sockd;
    bool _connected;
    bool autoreconnect;
    int reconnect_wait;
};

typedef struct _damn damn;

damn *damn_make(bool);
bool damn_connect(damn*);
void damn_disconnect(damn*);
char* damn_read(damn*);
ssize_t damn_write(damn*, char*);
void damn_finish(damn*);

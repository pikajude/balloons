#pragma once

#include <stdio.h>
#include "damn.h"
#include "settings.h"
#include "packet.h"
#include "protocol.h"

void set_damntoken(char*);

#define HANDLER(x) void handler_##x(damn *d, packet *p)

HANDLER(dAmnServer);
HANDLER(login);
HANDLER(ping);
HANDLER(property_members);
HANDLER(property_topic);
HANDLER(property_title);
HANDLER(property_privclasses);
HANDLER(recv_msg);

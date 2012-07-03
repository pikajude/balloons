#pragma once

#include <dlfcn.h>
#include <dirent.h>
#include "events.h"
#include "damn.h"
#include "packet.h"
#include "settings.h"

typedef void (*initfun)(events*);

unsigned long hook_msg(events*, bool, char*, damn_callback);
#define unhook_msg ev_unhook

void load_libs(events*);

void exec_commands(events*, damn*, packet*);

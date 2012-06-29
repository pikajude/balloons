#pragma once

#include <dlfcn.h>
#include <dirent.h>
#include "events.h"
#include "damn.h"
#include "packet.h"
#include "settings.h"

typedef void (*initfun)(events*);

void hook_msg(events*, char*, void(*)(damn*, packet*));

void load_libs(events*);

void exec_commands(events*, damn*, packet*);

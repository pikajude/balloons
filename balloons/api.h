#pragma once

#include <dlfcn.h>
#include <dirent.h>
#include "events.h"
#include "damn.h"
#include "packet.h"
#include "settings.h"

typedef struct {
    unsigned long (*hook_msg)(bool, char*, damn_callback);
    unsigned long (*hook_join)(damn_callback);
    unsigned long (*hook_part)(damn_callback);
    void (*unhook)(unsigned long);
} _api;

typedef void (*initfun)(_api*);
void load_libs(void);

void exec_commands(damn*, packet*);

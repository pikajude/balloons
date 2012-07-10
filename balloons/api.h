#pragma once

#include <dlfcn.h>
#include <dirent.h>
#include "events.h"
#include "damn.h"
#include "packet.h"
#include "settings.h"

typedef struct {
    bool triggered;
    char *name;
    damn_callback callback;
    unsigned char access;
} command;

typedef struct {
    unsigned long (*hook_msg)(command);
    unsigned long (*hook_join)(damn_callback);
    unsigned long (*hook_part)(damn_callback);
    void (*unhook)(unsigned long);
    events *events;
} _api;

typedef void (*initfun)(_api*);
void load_libs(void);

void exec_commands(damn*, packet*);

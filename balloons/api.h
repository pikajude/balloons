#pragma once

#include <dlfcn.h>
#include <dirent.h>
#include <wchar.h>
#include "events.h"
#include "damn.h"
#include "packet.h"
#include "settings.h"
#include "utils.h"
#include "protocol.h"
#include "chatenv.h"

typedef struct {
    bool triggered;
    bool async;
    unsigned int timeout;
    wchar_t *name;
    damn_callback callback;
    unsigned char access;
} command;

typedef struct {
    user *(*get_users)(wchar_t *);
} chatenv;

typedef struct {
    unsigned long (*hook_msg)(command);
    unsigned long (*hook_join)(damn_callback);
    unsigned long (*hook_part)(damn_callback);
    void (*unhook)(unsigned long);
    void (*setting_store)(wchar_t *, wchar_t *);
    chatenv *chatenv;
    events *events;
} _api;

typedef void (*initfun)(_api*);
void load_libs(void);

void exec_commands(damn*, packet*);

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "defs.h"

struct _arglist {
    wchar_t *key;
    wchar_t *value;
    struct _arglist *next;
};

typedef struct _arglist arglist;

arglist *al_make_pair(wchar_t*, wchar_t*);
void al_set(arglist*, wchar_t*, wchar_t*);
wchar_t *al_get(arglist*, wchar_t*);
wchar_t *al_get_def(arglist*, wchar_t*, wchar_t*);
void al_free(arglist*);
void al_print(arglist*);

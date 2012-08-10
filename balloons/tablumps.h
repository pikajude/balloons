#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <wchar.h>

struct _lump {
    wchar_t *find;
    wchar_t *repl;
    unsigned char arity;
    unsigned char groups[3];
};

typedef struct _lump lump;

wchar_t *delump(wchar_t *);

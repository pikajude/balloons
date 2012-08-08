#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

struct _lump {
    char *find;
    char *repl;
    unsigned char arity;
    unsigned char groups[3];
};

typedef struct _lump lump;

char *delump(char *);

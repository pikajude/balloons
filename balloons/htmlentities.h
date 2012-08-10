#pragma once

#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

struct _entity {
    char *name;
    wchar_t ord;
};

typedef struct _entity entity;

wchar_t *entity_decode(char *s);
char *entity_encode(char *s);

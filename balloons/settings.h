#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wordexp.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <wchar.h>
#include "alist.h"
#include "defs.h"

#define KEYLEN 65
#define VALLEN 2049
#define ARGFMT L"%64l[^:]: %2048l[^\n]\n"

typedef arglist settings;

settings *settings_all(void);
wchar_t *settings_dirname(void);
wchar_t *settings_filename(void);
void settings_load(bool);
void setting_store(wchar_t*, wchar_t*);
wchar_t *setting_get(wchar_t*);
int setting_exists(wchar_t*);

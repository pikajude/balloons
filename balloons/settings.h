#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>
#include "alist.h"
#include "defs.h"
#include <assert.h>

#define KEYLEN 65
#define VALLEN 513
#define ARGFMT "%64[^:]: %512[^\n]\n"

typedef arglist settings;

void setting_store(char*, char*);
char *setting_get(char*);
bool setting_exists(char*);

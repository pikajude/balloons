#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wordexp.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include "alist.h"
#include "defs.h"

#define KEYLEN 65
#define VALLEN 513
#define ARGFMT "%64[^:]: %512[^\n]\n"

typedef arglist settings;

settings *settings_all(void);
char *settings_dirname(void);
char *settings_filename(void);
void settings_load(bool);
void setting_store(char*, char*);
char *setting_get(char*);
int setting_exists(char*);

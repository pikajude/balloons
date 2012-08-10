#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <wchar.h>

enum _color {
    white   = 0,
    gray    = 30,
    red     = 31,
    green   = 32,
    yellow  = 33,
    blue    = 34,
    purple  = 35,
    cyan    = 36,
    bgray   = -30,
    bred    = -31,
    bgreen  = -32,
    byellow = -33,
    bblue   = -34,
    bpurple = -35,
    bcyan   = -36
};

typedef enum _color color;

void logger(color c, const wchar_t *msg, ...);

#define log_(x, ...) logger(white, (x), ##__VA_ARGS__)

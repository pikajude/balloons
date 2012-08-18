#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "protocol.h"

struct _context;

typedef struct {
    const wchar_t *cmd;
    bool *complete;
    void (*fun)(struct _context *);
    void *arg;
    unsigned int slp;
} cb;

void dispatch(const wchar_t *, unsigned int, void (*)(struct _context *), struct _context *);

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "protocol.h"

struct _context;

typedef struct {
    bool *complete;
    void (*fun)(struct _context *);
    void *arg;
} cb;

void dispatch(const char *, unsigned int, void (*)(struct _context *), struct _context *);
void thread_with_cleanup(cb *);
void on_complete(void *);

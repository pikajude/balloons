#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    bool *complete;
    void (*fun)(void *);
    void *arg;
} cb;

void dispatch(unsigned int, void (*)(void *), void *);
void thread_with_cleanup(cb *);
void on_complete(void *);

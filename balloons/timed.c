#include "timed.h"

void on_complete(void *b) {
    *((bool *)b) = true;
}

void thread_with_cleanup(cb *c) {
    pthread_cleanup_push(on_complete, c->complete);
    c->fun(c->arg);
    pthread_cleanup_pop(1);
}

void dispatch(unsigned int t, void (*fun)(void *), void *arg) {
    if(fork() == 0) {
        pthread_t thread;
        bool f = false;
        cb c;
        c.fun = fun;
        c.arg = arg;
        c.complete = &f;
        pthread_create(&thread, NULL, (void *(*)(void *))thread_with_cleanup, &c);
        sleep(t);
        pthread_cancel(thread);
        if(!f) {
            printf("Killed thread %p after %u seconds\n", thread, t);
        }
        exit(0);
    }
}

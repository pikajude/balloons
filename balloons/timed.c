#include "timed.h"
#include "events.h"

void on_complete(void *b) {
    *((bool *)b) = true;
}

void thread_with_cleanup(cb *c) {
    pthread_cleanup_push(on_complete, c->complete);
    c->fun(c->arg);
    pthread_cleanup_pop(1);
}

void dispatch(const wchar_t *cmd, unsigned int t, void (*fun)(context *), struct _context *arg) {
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
            dsendmsg(arg->damn, pkt_roomname(arg->pkt), L"Killed command <b>%ls</b> (%p) after %u seconds", cmd + 9, thread, t);
        }
        exit(0);
    }
}

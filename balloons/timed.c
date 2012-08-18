#include "timed.h"
#include "events.h"

static void on_complete(void *b) {
    *((bool *)b) = true;
}

static void thread_with_cleanup(cb *c) {
    pthread_cleanup_push(on_complete, c->complete);
    c->fun(c->arg);
    pthread_cleanup_pop(1);
}

static void *dispatch_handle(void *c) {
    pthread_t thread;
    bool f = false;
    cb *q = (cb *)c;
    q->complete = &f;
    pthread_create(&thread, NULL, (void *(*)(void *))thread_with_cleanup, c);
    sleep(q->slp);
    pthread_cancel(thread);
    struct _context *arg = q->arg;
    if(!f) {
        dsendmsg(arg->damn, pkt_roomname(arg->pkt), L"Killed command %ls (%p) after %u seconds", q->cmd, thread, q->slp);
    }
    free(c);
    arg->pkt->ref--;
    if(arg->pkt->ref == 0) {
        pkt_free(arg->pkt);
        free(arg);
    }
    return NULL;
}

void dispatch(const wchar_t *cmd, unsigned int t, void (*fun)(context *), struct _context *arg) {
    arg->pkt->ref++;
    pthread_t thread;
    cb *c = malloc(sizeof(cb));
    c->fun = fun;
    c->arg = arg;
    c->slp = t;
    c->cmd = cmd;
    pthread_create(&thread, NULL, dispatch_handle, c);
}

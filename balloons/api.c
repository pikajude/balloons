#include "api.h"

void hook_msg(void (*callback)(damn*, packet*)) {
    ev_hook(ev_get_global(), "pkt.recv.msg", callback);
    ev_hook(ev_get_global(), "pkt.recv.npmsg", callback);
}

void hook_act(void (*callback)(damn*, packet*)) {
    ev_hook(ev_get_global(), "pkt.recv.action", callback);
}

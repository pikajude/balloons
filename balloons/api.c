#include "api.h"

void hook_msg(events *e, char *command, void (*callback)(damn*, packet*)) {
    if (command == NULL) {
        ev_hook(e, "pkt.recv.msg", callback);
        ev_hook(e, "pkt.recv.npmsg", callback);
        ev_hook(e, "pkt.recv.action", callback);
    } else {
        char com[strlen(command) + 4];
        zero(com, strlen(command) + 4);
        sprintf(com, "cmd.%s", command);
        printf("%s\n", com);
        ev_hook(e, com, callback);
    }
}

void load_libs(events *e) {
    void *lib = dlopen("/Users/jdt/Code/C/balloons/testlib.so", RTLD_LAZY);
    initfun initializer = (initfun)dlsym(lib, "balloons_init");
    initializer(e);
}

void exec_commands(events *e, damn *d, packet *p) {
    if (strcmp(p->command, "recv") != 0) return;
    packet *sp = subpacket(p);
    char *trigger = setting_get("_trigger");
    if (strncmp(trigger, sp->body, strlen(trigger)) == 0) {
        char *bod = sp->body + strlen(trigger);
        size_t len = -1;
        while(bod[++len] > 32);
        char cmdname[len + 5];
        zero(cmdname, len + 5);
        snprintf(cmdname, len + 5, "cmd.%s", bod);
        ev_trigger(e, cmdname, d, p);
    }
}

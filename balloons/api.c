#include "api.h"

static const char *get_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (dot == NULL || dot == filename)
        return "";
    return dot + 1;
}

void hook_msg(events *e, char *command, void (*callback)(damn*, packet*)) {
    if (command == NULL) {
        ev_hook(e, "pkt.recv.msg", callback);
        ev_hook(e, "pkt.recv.npmsg", callback);
        ev_hook(e, "pkt.recv.action", callback);
    } else {
        char com[strlen(command) + 4];
        zero(com, strlen(command) + 4);
        sprintf(com, "cmd.%s", command);
        ev_hook(e, com, callback);
    }
}

void load_libs(events *e) {
    struct dirent *entry;
    DIR *extdir;
    void *lib;
    initfun initializer;
    const char *ext;
    char path[512] = { 0 };
    
    char *exts = setting_get(BKEY_EXTENSIONS_DIR);
    if (exts == NULL)
        return;
    
    extdir = opendir(exts);
    if (extdir == NULL) {
        perror("Couldn't open extension directory for reading. Are you sure it's readable and a directory?");
        return;
    }
    
    while ((entry = readdir(extdir))) {
        ext = get_extension(entry->d_name);
        if (strcmp(ext, "so") == 0) {
            zero(path, 512);
            snprintf(path, 511, "%s/%s", exts, entry->d_name);
            lib = dlopen(path, RTLD_LAZY);
            if (lib == NULL) {
                printf("Unable to read %s, invalid library\n", path);
                continue;
            }
            initializer = (initfun)dlsym(lib, BINIT_FUNCTION);
            if (initializer == NULL) {
                printf("Symbol %s not found in %s, might want to fix that.\n", BINIT_FUNCTION, path);
                continue;
            }
            initializer(e);
        }
    }
}

void exec_commands(events *e, damn *d, packet *p) {
    if (strcmp(p->command, "recv") != 0) return;
    packet *sp = subpacket(p);
    char *trigger = setting_get(BKEY_TRIGGER);
    if (strncmp(trigger, sp->body, strlen(trigger)) == 0) {
        char *bod = sp->body + strlen(trigger);
        size_t len = (unsigned long)-1;
        while(bod[++len] > 32);
        char *cmdname = calloc(1, len + 5);
        snprintf(cmdname, len + 5, "cmd.%s", bod);
        ev_trigger(e, cmdname, d, p);
    }
}

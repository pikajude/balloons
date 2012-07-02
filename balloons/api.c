#include "api.h"

static const char *get_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (dot == NULL || dot == filename)
        return "";
    return dot + 1;
}

void hook_msg(events *e, bool trigger, char *command, void (*callback)(damn*, packet*)) {
    if (!trigger) {
        char com[strlen(command) + 11];
        zero(com, strlen(command) + 11);
        sprintf(com, "cmd.notrig.%s", command);
        ev_hook(e, com, callback);
    } else {
        char com[strlen(command) + 9];
        zero(com, strlen(command) + 9);
        sprintf(com, "cmd.trig.%s", command);
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
        perror("Couldn't open extension directory");
        exit(EXIT_FAILURE);
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
    
    bool triggered = 0;
    char *bod;
    size_t len = 0;
    char *cmdname;
    char *uname = setting_get(BKEY_USERNAME);
    size_t uname_len = strlen(uname);
    packet *sp = pkt_subpacket(p);
    char *trigger = setting_get(BKEY_TRIGGER);
    
    if (strncmp(trigger, sp->body, strlen(trigger)) == 0) {
        triggered = true;
        bod = sp->body + strlen(trigger);
    } else if (strncmp(uname, sp->body, uname_len) == 0 &&
               sp->body[uname_len] == ':' &&
               sp->body[uname_len + 1] == ' ') {
        triggered = true;
        bod = sp->body + uname_len + 2;
    }
    
    if (triggered) {
        while (bod[len++] > 32);
        if (len > 1) {
            cmdname = calloc(1, len + 9);
            snprintf(cmdname, len + 9, "cmd.trig.%s", bod);
            ev_trigger(e, cmdname, d, p);
        }
    }
    
    char *ident = calloc(1, strlen(sp->body) + 11);
    sprintf(ident, "cmd.notrig.%s", sp->body);
    ev_trigger(e, ident, d, p);
}

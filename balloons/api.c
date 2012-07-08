#include "api.h"

static const char *get_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (dot == NULL || dot == filename)
        return "";
    return dot + 1;
}

static unsigned long hook_msg(bool trigger, char *command, damn_callback callback) {
    if (!trigger) {
        if (command == NULL) {
            return ev_hook("cmd.notrig", callback);
        } else {
            char com[strlen(command) + 11];
            zero(com, strlen(command) + 11);
            sprintf(com, "cmd.notrig.%s", command);
            return ev_hook(com, callback);
        }
    } else {
        char com[strlen(command) + 9];
        zero(com, strlen(command) + 9);
        sprintf(com, "cmd.trig.%s", command);
        return ev_hook(com, callback);
    }
}

static unsigned long hook_join(damn_callback callback) {
    return ev_hook("cmd.join", callback);
}

static unsigned long hook_part(damn_callback callback) {
    return ev_hook("cmd.part", callback);
}

void load_libs(void) {
    struct dirent *entry;
    DIR *extdir;
    void *lib;
    initfun initializer;
    const char *ext;
    char path[512] = { 0 };
    
    _api *a = malloc(sizeof(_api));
    a->hook_msg = hook_msg;
    a->hook_join = hook_join;
    a->hook_part = hook_part;
    a->unhook = ev_unhook;
    
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
            lib = dlopen(path, RTLD_NOW);
            if (lib == NULL) {
                printf("Unable to read %s, invalid library\n", path);
                continue;
            }
            initializer = (initfun)dlsym(lib, BINIT_FUNCTION);
            if (initializer == NULL) {
                printf("Symbol %s not found in %s, might want to fix that.\n", BINIT_FUNCTION, path);
                continue;
            }
            initializer(a);
        }
    }
    
    closedir(extdir);
}

void exec_commands(damn *d, packet *p) {
    if (strcmp(p->command, "recv") != 0) return;
    
    packet *sp = pkt_subpacket(p);
    if (sp->body == NULL) {
        if (strcmp(sp->command, "join") == 0) {
            ev_trigger("cmd.join", (context){d, p, NULL, sp->subcommand});
        } else if (strcmp(sp->command, "part") == 0) {
            ev_trigger("cmd.part", (context){d, p, NULL, sp->subcommand});
        }
        return;
    }
    
    bool triggered = 0;
    char *bod;
    size_t len = 0;
    char *cmdname;
    char *uname = setting_get(BKEY_USERNAME);
    size_t uname_len = strlen(uname);
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
            ev_trigger(cmdname, (context){d, p, bod + len, pkt_getarg(sp, "from") });
        }
    }
    
    context cbdata = { d, p, sp->body, pkt_getarg(sp, "from") };
    
    char *ident = calloc(1, strlen(sp->body) + 11);
    sprintf(ident, "cmd.notrig.%s", sp->body);
    ev_trigger(ident, cbdata);
    ev_trigger("cmd.notrig", cbdata);
}

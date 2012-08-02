#include "api.h"

static const char *get_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (dot == NULL || dot == filename)
        return "";
    return dot + 1;
}

static unsigned long hook_msg(command cmd) {
    assert(cmd.name == NULL || (cmd.name != NULL && strlen(cmd.name) < BCMDLEN_MAX));
    if (!cmd.triggered) {
        if (cmd.name == NULL) {
            return ev_hook("cmd.notrig", cmd.callback, cmd.access);
        } else {
            char com[strlen(cmd.name) + 11];
            zero(com, strlen(cmd.name) + 11);
            sprintf(com, "cmd.notrig.%s", cmd.name);
            return ev_hook(com, cmd.callback, cmd.access);
        }
    } else {
        assert(cmd.name != NULL);
        char com[strlen(cmd.name) + 9];
        zero(com, strlen(cmd.name) + 9);
        sprintf(com, "cmd.trig.%s", cmd.name);
        return ev_hook(com, cmd.callback, cmd.access);
    }
}

static unsigned long hook_join(damn_callback callback) {
    return ev_hookany("cmd.join", callback);
}

static unsigned long hook_part(damn_callback callback) {
    return ev_hookany("cmd.part", callback);
}

void load_libs(void) {
    struct dirent *entry;
    DIR *extdir;
    void *lib;
    initfun initializer;
    const char *ext;
    char path[512] = { 0 };
    
    _api *a = malloc(sizeof *a);
    if (a == NULL)
        HANDLE_ERR("Unable to allocate memory for _api");
    a->hook_msg = hook_msg;
    a->hook_join = hook_join;
    a->hook_part = hook_part;
    a->unhook = ev_unhook;
    a->events = ev_get_global();
    
    char *exts = setting_get(BKEY_EXTENSIONS_DIR);
    if (exts == NULL) {
        free(a);
        return;
    }
    
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
            ev_trigger("cmd.join", (context){d, p, NULL, sp->subcommand}, true);
        } else if (strcmp(sp->command, "part") == 0) {
            ev_trigger("cmd.part", (context){d, p, NULL, sp->subcommand}, true);
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
    
    char *sender = pkt_getarg(sp, "from");
    unsigned char senderaccess = sender == NULL ? 0 : access_get(sender);
    
    if (triggered) {
        while (bod[len++] > 32);
        if (len > 1) {
            cmdname = calloc(1, len + 9);
            if (cmdname == NULL)
                HANDLE_ERR("Unable to allocate command name");
            snprintf(cmdname, len + 9, "cmd.trig.%s", bod);
            ev_trigger_priv(cmdname, (context){d, p, bod + len, sender }, true, senderaccess);
        }
    }
    
    context cbdata = { d, p, sp->body, sender };
    
    char *ident = calloc(1, strlen(sp->body) + 11);
    if (ident == NULL)
        perror("Unable to allocate memory for command ID");
    sprintf(ident, "cmd.notrig.%s", sp->body);
    ev_trigger_priv(ident, cbdata, true, senderaccess);
    ev_trigger_priv("cmd.notrig", cbdata, true, senderaccess);
}

#include "api.h"

static const char *get_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (dot == NULL || dot == filename)
        return "";
    return dot + 1;
}

static unsigned long hook_msg(command cmd) {
    size_t len;
    assert(cmd.name == NULL || (cmd.name != NULL && wcslen(cmd.name) < BCMDLEN_MAX));
    if (!cmd.triggered) {
        if (cmd.name == NULL) {
            return ev_hook(L"cmd.notrig", cmd.callback, cmd.access, cmd.async, cmd.timeout);
        } else {
            len = wcslen(cmd.name) + 12;
            wchar_t com[len];
            wmemset(com, 0, len);
            swprintf(com, len, L"cmd.notrig.%ls", cmd.name);
            return ev_hook(com, cmd.callback, cmd.access, cmd.async, cmd.timeout);
        }
    } else {
        assert(cmd.name != NULL);
        len = wcslen(cmd.name) + 10;
        wchar_t com[len];
        wmemset(com, 0, len);
        swprintf(com, len, L"cmd.trig.%ls", cmd.name);
        return ev_hook(com, cmd.callback, cmd.access, cmd.async, cmd.timeout);
    }
}

static unsigned long hook_join(damn_callback callback) {
    return ev_hookany(L"cmd.join", callback);
}

static unsigned long hook_part(damn_callback callback) {
    return ev_hookany(L"cmd.part", callback);
}

void load_libs(void) {
    struct dirent *entry;
    DIR *extdir;
    void *lib;
    initfun initializer;
    const char *ext;
    wchar_t path[512] = { 0 };
    char *asciipath;
    
    _api *a = malloc(sizeof(_api));
    if (a == NULL)
        HANDLE_ERR("Unable to allocate memory for _api");
    a->hook_msg = hook_msg;
    a->hook_join = hook_join;
    a->hook_part = hook_part;
    a->unhook = ev_unhook;
    a->events = ev_get_global();
    a->setting_store = setting_store;
    
    wchar_t *exts = setting_get(BKEY_EXTENSIONS_DIR);
    if (exts == NULL) {
        free(a);
        return;
    }
    
    char *asciidir = calloc(1, wcslen(exts) * 4);
    wcstombs(asciidir, exts, wcslen(exts) * 4);
    
    extdir = opendir(asciidir);
    if (extdir == NULL) {
        perror("Couldn't open extension directory");
        exit(EXIT_FAILURE);
        return;
    }
    
    while ((entry = readdir(extdir))) {
        ext = get_extension(entry->d_name);
        if (strcmp(ext, "so") == 0) {
            wmemset(path, 0, 512);
            swprintf(path, 511, L"%s/%s", asciidir, entry->d_name);
            asciipath = calloc(1, 1022);
            wcstombs(asciipath, path, 1022);
            lib = dlopen(asciipath, RTLD_NOW);
            free(asciipath);
            if (lib == NULL) {
                wprintf(L"Unable to read %ls, invalid library\n", path);
                continue;
            }
            initializer = (initfun)dlsym(lib, BINIT_FUNCTION);
            if (initializer == NULL) {
                wprintf(L"Symbol %s not found in %ls, might want to fix that.\n", BINIT_FUNCTION, path);
                continue;
            }
            initializer(a);
        }
    }
    
    free(asciidir);
    closedir(extdir);
}

void exec_commands(damn *d, packet *p) {
    if (wcscmp(p->command, L"recv") != 0) return;
    
    packet *sp = pkt_subpacket(p);
    if (sp->body == NULL) {
        if (wcscmp(sp->command, L"join") == 0) {
            ev_trigger(L"cmd.join", (context){d, p, NULL, sp->subcommand});
        } else if (wcscmp(sp->command, L"part") == 0) {
            ev_trigger(L"cmd.part", (context){d, p, NULL, sp->subcommand});
        }
        return;
    }
    
    bool triggered = 0;
    wchar_t *bod;
    size_t len = 0;
    wchar_t *cmdname;
    wchar_t *uname = setting_get(BKEY_USERNAME);
    size_t uname_len = wcslen(uname);
    wchar_t *trigger = setting_get(BKEY_TRIGGER);
    
    if (wmemcmp(trigger, sp->body, wcslen(trigger)) == 0) {
        triggered = true;
        bod = sp->body + wcslen(trigger);
    } else if (wmemcmp(uname, sp->body, uname_len) == 0 &&
               sp->body[uname_len] == ':' &&
               sp->body[uname_len + 1] == ' ') {
        triggered = true;
        bod = sp->body + uname_len + 2;
    }
    
    wchar_t *sender = pkt_getarg(sp, L"from");
    unsigned char senderaccess = sender == NULL ? 0 : access_get(sender);
    
    if (triggered) {
        while (bod[len++] > 32);
        if (len > 1) {
            cmdname = calloc(1, sizeof(wchar_t) * (len + 10));
            if (cmdname == NULL)
                HANDLE_ERR("Unable to allocate command name");
            swprintf(cmdname, len + 9, L"cmd.trig.%ls", bod);
            ev_trigger_priv(cmdname, (context){d, p, bod + len, sender }, senderaccess);
        }
    }
    
    context cbdata = { d, p, sp->body, sender };
    
    wchar_t *ident = calloc(1, sizeof(wchar_t) * (wcslen(sp->body) + 13));
    if (ident == NULL)
        perror("Unable to allocate memory for command ID");
    swprintf(ident, wcslen(sp->body) + 12, L"cmd.notrig.%ls", sp->body);
    ev_trigger_priv(ident, cbdata, senderaccess);
    ev_trigger_priv(L"cmd.notrig", cbdata, senderaccess);
}

#include "settings.h"

static settings *current_settings = NULL;

settings *settings_all(void) {
    return current_settings;
}

wchar_t *settings_dirname(void) {
    wchar_t *fn = calloc(1, 512);
    if (fn == NULL)
        HANDLE_ERR("Unable to allocate space for home directory");
    char *home = getenv("HOME");
    swprintf(fn, 511, L"%s/.balloons", home);
    return fn;
}

wchar_t *settings_filename(void) {
    wchar_t *dirname = settings_dirname();
    wchar_t *fname = calloc(1, sizeof(wchar_t) * (wcslen(dirname) + 8));
    if (fname == NULL)
        HANDLE_ERR("Unable to allocate space for settings filename");
    wcscat(fname, dirname);
    wcscat(fname, L"/config");
    free(dirname);
    return fname;
}

void settings_load(bool reload) {
    if (current_settings != NULL && !reload) return;

    if (reload) {
        if (current_settings) al_free(current_settings);
        current_settings = NULL;
    }

    wchar_t *key = calloc(1, sizeof(wchar_t) * KEYLEN), *value = calloc(1, sizeof(wchar_t) * VALLEN);
    wchar_t *fname = settings_filename();
    char *asciifname = calloc(1, wcslen(fname) * 4);
    wcstombs(asciifname, fname, wcslen(fname) * 4);
    FILE *set = fopen(asciifname, "a+");
    free(fname);
    free(asciifname);
    if (set == NULL) {
        if (errno == ENOENT) {
            wchar_t *dname = settings_dirname();
            char *adirname = calloc(1, wcslen(dname));
            wcstombs(adirname, dname, wcslen(dname) * 2);
            if(mkdir(adirname, 0777) < 0) {
                perror("Unable to create settings directory");
                exit(EXIT_FAILURE);
            }
            free(dname);
            free(adirname);
            free(key);
            free(value);
            return settings_load(reload);
        } else {
            perror("Unable to open/create settings file");
            exit(EXIT_FAILURE);
        }
    }
    fseek(set, 0, SEEK_SET);

    while (fwscanf(set, ARGFMT, key, value) == 2) {
        if (current_settings == NULL)
            current_settings = al_make_pair(key, value);
        else
            al_set(current_settings, key, value);
        wmemset(key, 0, KEYLEN);
        wmemset(value, 0, VALLEN);
    }

    free(key);
    free(value);

    fclose(set);
}

void setting_store(wchar_t *key, wchar_t *value) {
    assert(!wcschr(key, L':'));
    assert(!wcschr(key, L'\n'));
    assert(!wcschr(value, L'\n'));
    if (current_settings == NULL)
        current_settings = al_make_pair(key, value);
    else
        al_set(current_settings, key, value);
    wchar_t *fname = settings_filename();
    char *afname = calloc(1, wcslen(fname));
    wcstombs(afname, fname, wcslen(fname) * 2);
    FILE *set = fopen(afname, "w");
    free(fname);
    free(afname);
    settings *s = current_settings;
    while (s != NULL) {
        fwprintf(set, L"%.64ls: %.2048ls\n", s->key, s->value);
        s = s->next;
    }
    fclose(set);
    settings_load(true);
}

wchar_t *setting_get(wchar_t *key) {
    if (current_settings == NULL) settings_load(true);
    return current_settings == NULL ? NULL : al_get(current_settings, key);
}

int setting_exists(wchar_t *key) {
    if (current_settings == NULL) settings_load(true);
    return current_settings == NULL ? false : al_get(current_settings, key) != NULL;
}

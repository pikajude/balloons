#include "settings.h"

static settings *current_settings = NULL;

char *settings_dirname(void) {
    char *fn = calloc(1, 512);
    if (fn == NULL)
        handle_err("Unable to allocate space for home directory");
    char *home = getenv("HOME");
    snprintf(fn, 511, "%s/.balloons", home);
    return fn;
}

char *settings_filename(void) {
    char *dirname = settings_dirname();
    char *fname = calloc(1, strlen(dirname) + 8);
    if (fname == NULL)
        handle_err("Unable to allocate space for settings filename");
    strcat(fname, dirname);
    strcat(fname, "/config");
    free(dirname);
    return fname;
}

void settings_load(int reload) {
    if (current_settings != NULL && !reload) return;
    
    if (reload) {
        if (current_settings) al_free(current_settings);
        current_settings = NULL;
    }
    
    char key[KEYLEN] = { 0 }, value[VALLEN] = { 0 };
    FILE *set = fopen(settings_filename(), "a+");
    if (set == NULL) {
        if (errno == ENOENT) {
            if(mkdir(settings_dirname(), 0777) < 0) {
                perror("Unable to create settings directory");
                exit(EXIT_FAILURE);
            }
            return settings_load(reload);
        } else {
            perror("Unable to open/create settings file");
            exit(EXIT_FAILURE);
        }
    }
    fseek(set, 0, SEEK_SET);
    
    while (fscanf(set, ARGFMT, key, value) == 2) {
        if (current_settings == NULL)
            current_settings = al_make_pair(key, value);
        else
            al_set(current_settings, key, value);
        zero(key, KEYLEN);
        zero(value, VALLEN);
    }
    
    fclose(set);
}

void setting_store(char *key, char *value) {
    assert(!strchr(key, ':'));
    assert(!strchr(key, '\n'));
    assert(!strchr(value, '\n'));
    if (current_settings == NULL)
        current_settings = al_make_pair(key, value);
    else
        al_set(current_settings, key, value);
    FILE *set = fopen(settings_filename(), "w");
    settings *s = current_settings;
    while (s != NULL) {
        fprintf(set, "%.64s: %.512s\n", s->key, s->value);
        s = s->next;
    }
    fclose(set);
    settings_load(true);
}

char *setting_get(char *key) {
    if (current_settings == NULL) settings_load(true);
    return current_settings == NULL ? NULL : al_get(current_settings, key);
}

int setting_exists(char *key) {
    if (current_settings == NULL) settings_load(true);
    return current_settings == NULL ? false : al_get(current_settings, key) != NULL;
}

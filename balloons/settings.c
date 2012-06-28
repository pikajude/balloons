#include "settings.h"

static settings *current_settings = NULL;

static char *settings_filename(void) {
    char *fn = malloc(200);
    char *home = getenv("HOME");
    snprintf(fn, 199, "%s/.balloons", home);
    return fn;
}

static void settings_load(bool reload) {
    if (current_settings != NULL && !reload) return;
    
    if (reload) {
        if (current_settings) al_free(current_settings);
        current_settings = NULL;
    }
    
    char key[KEYLEN] = { 0 }, value[VALLEN] = { 0 };
    FILE *set = fopen(settings_filename(), "a+");
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

bool setting_exists(char *key) {
    if (current_settings == NULL) settings_load(true);
    return current_settings == NULL ? false : al_get(current_settings, key) != NULL;
}

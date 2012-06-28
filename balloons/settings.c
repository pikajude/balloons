#include "settings.h"

static char *settings_filename(void) {
    char *fn = malloc(200);
    char *home = getenv("HOME");
    snprintf(fn, 199, "%s/.balloons", home);
    return fn;
}

static settings *settings_load(void) {
    settings *s = NULL;
    char key[KEYLEN] = { 0 }, value[VALLEN] = { 0 };
    FILE *set = fopen(settings_filename(), "a+");
    fseek(set, 0, SEEK_SET);
    
    while (fscanf(set, ARGFMT, key, value) == 2) {
        if (s == NULL)
            s = al_make_pair(key, value);
        else
            al_set(s, key, value);
        zero(key, KEYLEN);
        zero(value, VALLEN);
    }
    
    fclose(set);
    return s;
}

void setting_store(char *key, char *value) {
    assert(!strchr(key, ':'));
    assert(!strchr(key, '\n'));
    assert(!strchr(value, '\n'));
    settings *s = settings_load();
    if (s == NULL)
        s = al_make_pair(key, value);
    else
        al_set(s, key, value);
    FILE *set = fopen(settings_filename(), "w");
    while (s != NULL) {
        fprintf(set, "%.64s: %.512s\n", s->key, s->value);
        s = s->next;
    }
    fclose(set);
}

char *setting_get(char *key) {
    settings *s = settings_load();
    return s == NULL ? NULL : al_get(s, key);
}

bool setting_exists(char *key) {
    settings *s = settings_load();
    return s == NULL ? false : al_get(s, key) != NULL;
}

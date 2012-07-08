#include "alist.h"

static void al_append(arglist *a, char *key, char *value) {
    while(a->next != NULL) a = a->next;
    a->next = al_make_pair(key, value);
}

static void al_keyset(arglist *a, char *key) {
    unsigned long len = strlen(key) + 1;
    a->key = realloc(a->key, len);
    if (a->key == NULL)
        handle_err("Unable to allocate memory for key");
    strncpy(a->key, key, len);
}

static void al_valueset(arglist *a, char *val) {
    unsigned long len = strlen(val) + 1;
    a->value = realloc(a->value, len);
    if (a->value == NULL)
        handle_err("Unable to allocate memory for value");
    strncpy(a->value, val, len);
}

arglist *al_make_pair(char *key, char *value) {
    arglist *a = calloc(1, sizeof *a);
    if (a == NULL)
        handle_err("Unable to allocate memory for arglist");
    al_keyset(a, key);
    al_valueset(a, value);
    return a;
}

void al_set(arglist *a, char *key, char *value) {
    if (a == NULL) return;
    arglist *cur = a;
    do {
        if (strncmp(cur->key, key, strlen(key)) == 0) {
            al_keyset(cur, key);
            al_valueset(cur, value);
            return;
        }
        cur = cur->next;
    } while (cur != NULL);
    al_append(a, key, value);
}

char *al_get(arglist *a, char *key) {
    do {
        if (strcmp(a->key, key) == 0)
            return a->value;
    } while ((a = a->next) != NULL);
    return NULL;
}

char *al_get_def(arglist *a, char *key, char *default_val) {
    char *res = al_get(a, key);
    return res == NULL ? default_val : res;
}

void al_free(arglist *a) {
    if (a->next != NULL)
        al_free(a->next);
    free(a->key);
    free(a->value);
    free(a);
}

void al_print(arglist *a) {
    printf("args = {");
    while (a != NULL) {
        printf("\n   %s: %s", a->key, a->value);
        a = a->next;
        if (a == NULL)
            printf("\n");
    }
    printf("}\n");
}

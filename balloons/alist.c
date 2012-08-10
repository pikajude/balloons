#include "alist.h"

static void al_append(arglist *a, wchar_t *key, wchar_t *value) {
    while(a->next != NULL) a = a->next;
    a->next = al_make_pair(key, value);
}

static void al_keyset(arglist *a, wchar_t *key) {
    unsigned long len = wcslen(key) + 1;
    a->key = realloc(a->key, sizeof(wchar_t) * len);
    if (a->key == NULL)
        HANDLE_ERR("Unable to allocate memory for key");
    wcsncpy(a->key, key, len);
}

static void al_valueset(arglist *a, wchar_t *val) {
    unsigned long len = wcslen(val) + 1;
    a->value = realloc(a->value, sizeof(wchar_t) * len);
    if (a->value == NULL)
        HANDLE_ERR("Unable to allocate memory for value");
    wcsncpy(a->value, val, len);
}

arglist *al_make_pair(wchar_t *key, wchar_t *value) {
    arglist *a = calloc(1, sizeof(arglist));
    if (a == NULL)
        HANDLE_ERR("Unable to allocate memory for arglist");
    al_keyset(a, key);
    al_valueset(a, value);
    return a;
}

void al_set(arglist *a, wchar_t *key, wchar_t *value) {
    if (a == NULL) return;
    arglist *cur = a;
    do {
        if (wcsncmp(cur->key, key, wcslen(key)) == 0) {
            al_keyset(cur, key);
            al_valueset(cur, value);
            return;
        }
        cur = cur->next;
    } while (cur != NULL);
    al_append(a, key, value);
}

wchar_t *al_get(arglist *a, wchar_t *key) {
    do {
        if (wcscmp(a->key, key) == 0)
            return a->value;
    } while ((a = a->next) != NULL);
    return NULL;
}

wchar_t *al_get_def(arglist *a, wchar_t *key, wchar_t *default_val) {
    wchar_t *res = al_get(a, key);
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
        wprintf(L"\n   %ls: %ls", a->key, a->value);
        a = a->next;
        if (a == NULL)
            printf("\n");
    }
    printf("}\n");
}

#include "utils.h"

static void swap(const void **a, const void **b) {
    uintptr_t holdera = (uintptr_t)*a;
    *a = *b;
    *b = (void*)holdera;
}

void quicksort(void **arr, int beg, int end, int comparator(const void *, const void *)) {
    if (end > beg + 1) {
        void *piv = arr[beg];
        int l = beg + 1, r = end;
        while (l < r) {
            if (comparator(arr[l], piv) <= 0)
                l++;
            else
                swap((const void **)&arr[l], (const void **)&arr[--r]);
        }
        swap((const void **)&arr[--l], (const void **)&arr[beg]);
        quicksort(arr, beg, l, comparator);
        quicksort(arr, r, end, comparator);
    }
}

unsigned char access_get(char *uname) {
    unsigned char access;
    char *setting_name = calloc(1, strlen(uname) + 8);
    strcpy(setting_name, "access.");
    strcat(setting_name, uname);
    
    char *setting = setting_get(setting_name);
    if (setting == NULL)
        access = 0;
    else
        sscanf(setting, "%d", &access);
    return access;
}

void access_store(char *uname, unsigned char access) {
    char *acbuf = calloc(1, 3);
    char *setting_name = calloc(1, strlen(uname) + 8);
    strcpy(setting_name, "access.");
    strcat(setting_name, uname);
    sprintf(acbuf, "%d", access);
    setting_store(setting_name, acbuf);
}

unsigned char access_get_cmd(events *e, char *cmdname) {
    events *cur = e;
    while (cur != NULL) {
        if (strcmp(cur->name + 9, cmdname) == 0 || strcmp(cur->name + 11, cmdname) == 0)
            return cur->access;
        cur = cur->next;
    }
    return 255;
}

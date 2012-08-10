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

unsigned char access_get(wchar_t *uname) {
    unsigned int _access;
    wchar_t *setting_name = calloc(1, (wcslen(uname) + 8) * sizeof(wchar_t));
    swprintf(setting_name, wcslen(uname) + 8, L"access.%ls", uname);
    
    wchar_t *setting = setting_get(setting_name);
    if (setting == NULL)
        _access = 0;
    else
        swscanf(setting, L"%d", &_access);
    return (unsigned char)_access;
}

void access_store(char *uname, unsigned char _access) {
    char *acbuf = calloc(1, 3);
    char *setting_name = calloc(1, strlen(uname) + 8);
    sprintf(setting_name, "access.%s", uname);
    sprintf(acbuf, "%d", _access);
    setting_store((wchar_t *)setting_name, (wchar_t *)acbuf);
}

unsigned char access_get_cmd(events *e, wchar_t *cmdname) {
    events *cur = e;
    while (cur != NULL) {
        if (wcscmp(cur->name + 9, cmdname) == 0 || wcscmp(cur->name + 11, cmdname) == 0)
            return cur->access;
        cur = cur->next;
    }
    return 255;
}

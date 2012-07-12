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
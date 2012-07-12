#pragma once

#include "alist.h"
#include "settings.h"

void quicksort(void **, int, int, int comparator(const void *, const void *));
unsigned char access_get(char *uname);
void access_store(char *uname, unsigned char access);

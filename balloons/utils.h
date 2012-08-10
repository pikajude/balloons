#pragma once

#include "alist.h"
#include "settings.h"
#include "events.h"

void quicksort(void **, int, int, int comparator(const void *, const void *));
unsigned char access_get(wchar_t *);
void access_store(char *, unsigned char);
unsigned char access_get_cmd(events *, wchar_t *);

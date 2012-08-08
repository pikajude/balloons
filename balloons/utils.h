#pragma once

#include "packet/alist.h"
#include "settings.h"
#include "events.h"

void quicksort(void **, int, int, int comparator(const void *, const void *));
unsigned char access_get(char *);
void access_store(char *, unsigned char);
unsigned char access_get_cmd(events *, char *);

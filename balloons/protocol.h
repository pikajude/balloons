#pragma once

#include <unistd.h>
#include "damn.h"

#define writestr(a,b) send(a, (b), strlen(b), 0)
#define finish(a) send(a, "\n", 2, 0)

void phandshake(damn*);
void plogin(damn*, char*, char*);
void pjoin(damn*, char*);
void psendmsg(damn*, char*, char*);

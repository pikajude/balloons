#pragma once

#include <unistd.h>
#include "damn.h"

#define writestr(a,b) send(a, (b), strlen(b), 0)
#define finish(a) send(a, "\n", 2, 0)

void dhandshake(damn*);
void dlogin(damn*, char*, char*);
void djoin(damn*, char*);
void dsendmsg(damn*, char*, char*);

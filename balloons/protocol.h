#pragma once

#include <unistd.h>
#include "damn.h"

#define writestr(a,b) write(a, (b), strlen(b))
#define finish(a) write(a, "\n", 2)

void handshake(damn*);
void login(damn*, char*, char*);

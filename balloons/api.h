#pragma once

#include "events.h"
#include "damn.h"
#include "packet.h"

void hook_msg(void(*)(damn*, packet*));
void hook_act(void(*)(damn*, packet*));

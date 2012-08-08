#pragma once

#include <unistd.h>
#include <stdarg.h>
#include "damn.h"

void dhandshake(struct _damn*);
void dlogin(struct _damn*, char*, char*);
void djoin(struct _damn*, char*);
void dpong(struct _damn*);
void dsendmsgtype(struct _damn*, char*, char*, char*, ...);

#define dsendmsg(d,room,msg,...) dsendmsgtype(d,"msg",room,msg,##__VA_ARGS__)
#define dsendnpmsg(d,room,msg,...) dsendmsgtype(d,"npmsg",room,msg,##__VA_ARGS__)
#define dsendaction(d,room,msg,...) dsendmsgtype(d,"action",room,msg,##__VA_ARGS__)
#define finish(a) send(a, "\n", 2, 0)

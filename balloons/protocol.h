#pragma once

#include <unistd.h>
#include <stdarg.h>
#include <locale.h>
#include <assert.h>
#include "damn.h"
#include "htmlentities.h"

void dhandshake(struct _damn*);
void dlogin(struct _damn*, wchar_t*, wchar_t*);
void djoin(struct _damn*, wchar_t*);
void dpong(struct _damn*);
void dsendmsgtype(struct _damn*, char*, wchar_t*, wchar_t*, ...);

#define dsendmsg(d,room,msg,...) dsendmsgtype(d,"msg",room,msg,##__VA_ARGS__)
#define dsendnpmsg(d,room,msg,...) dsendmsgtype(d,"npmsg",room,msg,##__VA_ARGS__)
#define dsendaction(d,room,msg,...) dsendmsgtype(d,"action",room,msg,##__VA_ARGS__)
#define finish(a) send(a, "\n", 2, 0)

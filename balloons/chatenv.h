#pragma once

#include <stdlib.h>
#include "packet.h"

struct _user {
    wchar_t *name;
    short joincount;
    struct _user *next;
};

typedef struct _user user;

struct _room {
    wchar_t *name;
    wchar_t *topic;
    wchar_t *title;
    user *users;
    struct _room *next;
};

typedef struct _room room;

void add_room(wchar_t *name);
void rset_title(wchar_t *name, wchar_t *title);
void rset_topic(wchar_t *name, wchar_t *topic);
void radd_users(wchar_t *name, packet *p);
void radd_user(wchar_t *name, wchar_t *user);
void rdel_user(wchar_t *name, wchar_t *user);
void delete_room(wchar_t *name);

user *rget_users(wchar_t *name);

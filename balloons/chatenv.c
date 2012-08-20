#include "chatenv.h"

static room *roomstore = NULL;

static void ufree(user *u) {
    if(u->next != NULL)
        ufree(u->next);
    free(u->name);
    free(u);
}

static void rfree(room *r) {
    free(r->title);
    free(r->topic);
    free(r->name);
    ufree(r->users);
    free(r);
}

static void add_user(room *r, wchar_t *name) {
    user *new = calloc(1, sizeof(user)), *cur = r->users;
    new->name = wcsdup(name);
    new->joincount = 1;
    if(cur == NULL) {
        r->users = new;
        return;
    }
    for(;;) {
        if(wcscmp(cur->name, name) == 0) {
            cur->joincount++;
            free(new->name);
            free(new);
            return;
        }
        if(cur->next == NULL) break;
        cur = cur->next;
    }
    cur->next = new;
}

static void del_user(room *r, wchar_t *name) {
    user *cur = r->users;
    if(wcscmp(cur->name, name) == 0) {
        r->users = r->users->next;
        ufree(cur);
        return;
    }
    do {
        if(wcscmp(cur->next->name, name) == 0) {
            if(cur->next->joincount < 2) {
                user *next = cur->next;
                cur->next = next->next;
                next->next = NULL;
                ufree(next);
            } else {
                cur->next->joincount--;
            }
            return;
        }
    } while ((cur = cur->next) != NULL);
}

void add_room(wchar_t *name) {
    room *r = calloc(1, sizeof(room));
    r->name = wcsdup(name);
    room *cur = roomstore;
    if(cur == NULL)
        roomstore = r;
    else {
        while(cur->next != NULL) cur = cur->next;
        cur->next = r;
    }
}

void delete_room(wchar_t *name) {
    room *cur = roomstore;
    if(wcscmp(cur->name, name) == 0) {
        roomstore = cur->next;
        rfree(cur);
        return;
    }
    do {
        if(wcscmp(cur->next->name, name)) {
            room *next = cur->next;
            cur->next = next->next;
            rfree(next);
            return;
        }
    } while((cur = cur->next) != NULL);
}

void rset_title(wchar_t *name, wchar_t *title) {
    room *cur = roomstore;
    do {
        if(wcscmp(cur->name, name) == 0) {
            cur->title = wcsdup(title);
            return;
        }
    } while ((cur = cur->next) != NULL);
}

void rset_topic(wchar_t *name, wchar_t *topic) {
    room *cur = roomstore;
    do {
        if(wcscmp(cur->name, name) == 0) {
            cur->topic = wcsdup(topic);
            return;
        }
    } while ((cur = cur->next) != NULL);
}

void radd_users(wchar_t *name, packet *p) {
    room *cur = roomstore;
    packet *s = p;
    for(;;) {
        if(wcscmp(cur->name, name) == 0)
            break;
        cur = cur->next;
        if(cur == NULL)
            return;
    }
    while((s = pkt_subpacket(s))->subcommand != NULL)
        add_user(cur, s->subcommand);
}

user *rget_users(wchar_t *name) {
    room *cur = roomstore;
    do {
        if(wcscmp(cur->name, name) == 0)
            return cur->users;
    } while ((cur = cur->next) != NULL);
    return NULL;
}

void radd_user(wchar_t *name, wchar_t *user) {
    room *cur = roomstore;
    do {
        if(wcscmp(cur->name, name) == 0) {
            add_user(cur, user);
            return;
        }
    } while ((cur = cur->next) != NULL);
}

void rdel_user(wchar_t *name, wchar_t *user) {
    room *cur = roomstore;
    do {
        if(wcscmp(cur->name, name) == 0) {
            del_user(cur, user);
            return;
        }
    } while ((cur = cur->next) != NULL);
}

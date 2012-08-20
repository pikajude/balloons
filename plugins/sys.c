#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <wchar.h>
#ifndef __WIN32__
#include <sys/utsname.h>
#endif
#ifdef __APPLE__
#include <mach/mach.h>
#endif
#include "api.h"
#include "alist.h"

struct access_pair {
    wchar_t *user;
    wchar_t *level;
};

static _api *api;
static unsigned long pingsendid = 0, pinghookid = 0;
static long microseconds = 0;

static long microtime(void) {
    long us = 0;
    struct timeval t;
    gettimeofday(&t, NULL);
    us += t.tv_sec * 1000000;
    us += t.tv_usec;
    return us;
}

static int cmp_events(const void *e1, const void *e2) {
    return wcscmp(((events *)e1)->name, ((events *)e2)->name);
}

static int cmp_str(const void *e1, const void *e2) {
    return wcscmp((wchar_t *)e1, (wchar_t *)e2);
}

static int cmp_strint(const void *s1, const void *s2) {
    wchar_t *str1 = ((struct access_pair *)s1)->level, *str2 = ((struct access_pair *)s2)->level;
    size_t l1 = wcslen(str1), l2 = wcslen(str2), i;
    if (l1 != l2)
        return l1 > l2 ? 1 : -1;
    for (i = 0; i < l1; i++) {
        if (str1[i] > str2[i]) {
            return 1;
        } else if (str1[i] < str2[i]) {
            return -1;
        }
    }
    return 1;
}

static void about(context *ctx) {
#ifdef __WIN32__
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"<b>balloons</b> version %ls, written by <b>incluye</b>, running C99 on Windows", BVERSION);
#else
    struct utsname u;
    uname(&u);
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"<b>balloons</b> version %ls, written by <b>incluye</b>, running C99 on %s %s", BVERSION, u.sysname, u.release);
#endif
}

static void memuse(context *ctx) {
#ifdef __APPLE__
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof info;
    task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size);
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"Memory usage in bytes: %lu", info.resident_size);
#endif
}

static void trigcheck(context *ctx) {
    wchar_t *uname = api->setting_get(BKEY_USERNAME);
	size_t len = wcslen(uname) + 12;
    wchar_t *tc = calloc(1, sizeof(wchar_t) * len);
    swprintf(tc, len, L"%ls: trigcheck", uname);
    if (wcsstr(ctx->msg, tc) != NULL)
        dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"%ls: %ls (or '%ls: ')", ctx->sender, api->setting_get(BKEY_TRIGGER), api->setting_get(BKEY_USERNAME));
    free(tc);
}

static void botcheck(context *ctx) {
    wchar_t *uname = api->setting_get(BKEY_USERNAME);
	size_t len = wcslen(uname) + 11;
    wchar_t *bc = calloc(1, sizeof(wchar_t) * len);
    swprintf(bc, len, L"%ls: botcheck", uname);
    if (wcsstr(ctx->msg, bc) != NULL)
        dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"%ls: I'm a bot!", ctx->sender);
    free(bc);
}

static void ping(context *);

static void pong(context *ctx) {
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"pong! (%ldms)", (microtime() - microseconds) / 1000);
    api->unhook(pingsendid);
    pingsendid = microseconds = 0;
    pinghookid = api->hook_msg((command){ .triggered = true, .name = L"ping", .callback = &ping, .async = true });
} 

static void ping(context *ctx) {
    api->unhook(pinghookid);
    pingsendid = api->hook_msg((command){ .triggered = false, .name = L"ping?", .callback = &pong, .async = true });
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"ping?");
    microseconds = microtime();
}

static void echo(context *ctx) {
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), ctx->msg);
}

static void commands(context *ctx) {
    size_t idx = 0, fullsize = 24, i = (size_t)-1, j;
    unsigned char access = access_get(ctx->sender);
    
    events **commands = calloc(1, fullsize * sizeof(events*));
    events *cur = api->events;
    do {
        if (wcsncmp(cur->name, L"cmd.trig", 8) == 0)
            commands[idx++] = cur;
        if (idx >= fullsize) {
            fullsize += 8;
            commands = realloc(commands, fullsize * sizeof(events*));
            if (commands == NULL)
                HANDLE_ERR("couldn't resize commands");
        }
    } while ((cur = cur->next) != NULL);
    while (commands[++i] != NULL);
    quicksort((void **)commands, 0, (int)i, cmp_events);
    
    wchar_t *msgstr = malloc(sizeof(wchar_t) * (10 + i * (BCMDLEN_MAX + 8)));
    wcscpy(msgstr, L"Commands: ");
    
    for(j = 0; j < i; j++) {
        if (access < commands[j]->access)
            wcscat(msgstr, L"<i>");
        wcscat(msgstr, commands[j]->name + 9);
        if (access < commands[j]->access)
            wcscat(msgstr, L"</i>");
        wcscat(msgstr, L" ");
    }
    
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), msgstr);
    free(commands);
}

static void can(context *ctx) {
    wchar_t uname[128], cmd[128];
    unsigned char uaccess, caccess;
    if (swscanf(ctx->msg, L"%ls use %l[^? ]?", uname, cmd) < 2) {
        dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"I don't know.");
    } else {
        caccess = access_get_cmd(api->events, cmd);
        if (wcscmp(uname, L"everyone") == 0) {
            dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), caccess == 0 ? L"Yes." : L"No.");
        } else {
            if (wcscmp(uname, L"I") == 0 || wcscmp(uname, L"i") == 0)
                wcscpy(uname, ctx->sender);
            uaccess = access_get(uname);
            dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), uaccess >= caccess ? L"Yes." : L"No.");
        }
    }
}

static void laccess(context *ctx) {
    size_t fullsize = 24, cur = 0;
    struct access_pair *pairs = calloc(1, fullsize * sizeof(struct access_pair));
    settings *s = settings_all();
    while (s != NULL) {
        if (wcsncmp(s->key, L"access.", 7) == 0) {
            pairs[cur++] = (struct access_pair){s->key + 7, s->value};
        }
        if (cur == fullsize - 1) {
            fullsize += 8;
            pairs = realloc(pairs, fullsize * sizeof(struct access_pair));
            if (pairs == NULL)
                HANDLE_ERR("Couldn't allocate pairs");
        }
        s = s->next;
    }
    quicksort((void **)&pairs, 0, (int)cur, cmp_strint);
    
    wchar_t *msgstr = calloc(1, sizeof(wchar_t) * (cur * 26));
    for (fullsize = 0; fullsize < cur; fullsize++) {
        wcscat(msgstr, pairs[fullsize].user);
        wcscat(msgstr, L"(");
        wcscat(msgstr, pairs[fullsize].level);
        wcscat(msgstr, L") ");
    }
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), msgstr);
    free(pairs);
}

static void die(context *ctx) {
    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"Exiting.");
    exit(0);
}

static void autojoin(context *ctx) {
    size_t idx = 0;
    int j, k;
    wchar_t *msg = ctx->msg;
    wchar_t **rooms = NULL, *split, *room, *new, *sp, *target, *set = wcsdup(api->setting_get(BKEY_AUTOJOIN));
    room = wcstok(set, L" ", &split);
    do {
        rooms = realloc(rooms, sizeof(wchar_t *) * (idx + 1));
        rooms[idx++] = room[0] == L'#' ? room + 1 : room;
    } while ((room = wcstok(NULL, L" ", &split)) != NULL);
    
    quicksort((void **)rooms, 0, (int)idx, cmp_str);
    
    if(wcsncmp(msg, L"list", 4) == 0) {
        size_t len = 0;
        int i = 0;
        for(; i < idx; i++) {
            len += wcslen(rooms[i]) + 2;
        }
        wchar_t *list = calloc(1, sizeof(wchar_t) * (len + 1));
        for(i = 0; i < idx; i++) {
            wcscat(list, L"#");
            wcscat(list, rooms[i]);
            wcscat(list, L" ");
        }
        dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), list);
    } else if(wcsncmp(msg, L"add", 3) == 0) {
        if(wcslen(msg) > 4 && msg[3] == L' ') {
            new = wcsdup(msg + 4 + (msg[4] == '#'));
            sp = wcschr(new, L' ');
            if(sp != NULL)
                sp[0] = '\0';
            for(j = 0; j < idx; j++) {
                if(wcscmp(rooms[j], new) == 0) {
                    dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"That room is already in the list!");
                    goto done;
                }
            }
            rooms = realloc(rooms, sizeof(wchar_t *) * (idx + 1));
            rooms[idx] = new;
            target = calloc(1, 2048 * sizeof(wchar_t));
            for(k = 0; k <= idx; k++) {
                wcscat(target, rooms[k]);
                if(k < idx)
                    wcscat(target, L" ");
            }
            api->setting_store(BKEY_AUTOJOIN, target);
            free(target);
            dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"Done.");
        } else {
            dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"Usage: autojoin add <i>room</i>");
        }
    } else if(wcsncmp(msg, L"del", 3) == 0) {
        if(wcslen(msg) > 4 && msg[3] == L' ') {
            new = wcsdup(msg + 4 + (msg[4] == '#'));
            sp = wcschr(new, L' ');
            bool found;
            if(sp != NULL)
                sp[0] = '\0';
            for(j = 0; j < idx; j++) {
                if(wcscmp(rooms[j], new) == 0) {
                    rooms[j] = NULL;
                    found = true;
                }
            }
            if(!found) {
                dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"That room isn't in the list.");
                goto done;
            }
            target = calloc(1, 2048 * sizeof(wchar_t));
            for(k = 0; k < idx; k++) {
                if(rooms[k] == NULL) continue;
                wcscat(target, rooms[k]);
                if(k < idx)
                    wcscat(target, L" ");
            }
            api->setting_store(BKEY_AUTOJOIN, target);
            free(target);
            dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"Done.");
        } else {
            dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"Usage: autojoin del <i>room</i>");
        }
    } else {
        dsendmsg(ctx->damn, pkt_roomname(ctx->pkt), L"Usage: autojoin (add <i>room</i> | del <i>room</i> | list)");
    }
    
done:
    free(set);
    free(rooms);
}

void balloons_init(_api *a) {
    api = a;
    api->hook_msg((command){ .callback = &trigcheck });
    api->hook_msg((command){ .callback = &botcheck });
    pinghookid = api->hook_msg((command){ .triggered = true, .name = L"ping", .callback = &ping, .async = true });
    api->hook_msg((command){ .triggered = true, .name = L"echo", .callback = &echo, .access = 1 });
    api->hook_msg((command){ .triggered = true, .name = L"about", .callback = &about });
    api->hook_msg((command){ .triggered = true, .name = L"autojoin", .callback = &autojoin, .access = 254 });
    api->hook_msg((command){ .triggered = true, .name = L"commands", .callback = &commands });
    api->hook_msg((command){ .triggered = true, .name = L"can", .callback = &can });
    api->hook_msg((command){ .triggered = true, .name = L"access", .callback = &laccess });
    api->hook_msg((command){ .triggered = true, .name = L"memuse", .callback = &memuse });
    api->hook_msg((command){ .triggered = true, .name = L"quit", .callback = &die, .access = 254 });
}

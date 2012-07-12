#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#ifndef __WIN32__
    #include <sys/utsname.h>
#endif
#include "api.h"
#include "protocol.h"
#include "events.h"
#include "utils.h"

#define ANYONE 0

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
    return strcmp(((events *)e1)->name, ((events *)e2)->name);
}

static void about(context ctx) {
#ifdef __WIN32__
    dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), "<b>balloons</b> version %s, written by <b>incluye</b>, running C99 on Windows", BVERSION);
#else
    struct utsname u;
    uname(&u);
    dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), "<b>balloons</b> version %s, written by <b>incluye</b>, running C99 on %s %s", BVERSION, u.sysname, u.release);
#endif
}

static void trigcheck(context ctx) {
    char *uname = setting_get(BKEY_USERNAME);
    char *tc = calloc(1, strlen(uname) + 12);
    sprintf(tc, "%s: trigcheck", uname);
    if (strstr(ctx.msg, tc) != NULL)
        dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), "%s: %s", ctx.sender, setting_get(BKEY_TRIGGER));
}

static void botcheck(context ctx) {
    char *uname = setting_get(BKEY_USERNAME);
    char *bc = calloc(1, strlen(uname) + 11);
    sprintf(bc, "%s: botcheck", uname);
    if (strstr(ctx.msg, bc) != NULL)
        dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), "%s: I'm a bot!", ctx.sender);
}

static void ping(context);

static void pong(context ctx) {
    dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), "pong! (%ldms)", (microtime() - microseconds) / 1000);
    api->unhook(pingsendid);
    pingsendid = microseconds = 0;
    pinghookid = api->hook_msg((command){ .triggered = true, .name = "ping", .callback = &ping });
} 

static void ping(context ctx) {
    api->unhook(pinghookid);
    pingsendid = api->hook_msg((command){ .triggered = false, .name = "ping?", .callback = &pong });
    dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), "ping?");
    microseconds = microtime();
}

static void echo(context ctx) {
    dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), ctx.msg);
}

static void commands(context ctx) {
    size_t idx = 0, fullsize = 24, i = (size_t)-1, j;
    unsigned char access = access_get(ctx.sender);
    
    events **commands = calloc(1, fullsize * sizeof(char*));
    events *cur = api->events;
    do {
        if (strncmp(cur->name, "cmd.trig", 8) == 0)
            commands[idx++] = cur;
        if (idx >= fullsize) {
            fullsize += 8;
            commands = realloc(commands, fullsize * sizeof(char*));
            if (commands == NULL)
                handle_err("couldn't resize commands");
        }
    } while ((cur = cur->next) != NULL);
    while (commands[++i] != NULL);
    quicksort((void **)commands, 0, (int)i, cmp_events);
    
    char *msgstr = malloc(10 + (i * (BCMDLEN_MAX + 7)) + i);
    strcpy(msgstr, "Commands: ");
    
    for(j = 0; j < i; j++) {
        if (access < commands[j]->access)
            strcat(msgstr, "<i>");
        strcat(msgstr, commands[j]->name + 9);
        if (access < commands[j]->access)
            strcat(msgstr, "</i>");
        strcat(msgstr, " ");
    }
    
    dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), msgstr);
}

void balloons_init(_api *a) {
    api = a;
    api->hook_msg((command){ .callback = &trigcheck });
    api->hook_msg((command){ .callback = &botcheck });
    pinghookid = api->hook_msg((command){ .triggered = true, .name = "ping", .callback = &ping });
    api->hook_msg((command){ .triggered = true, .name = "echo", .callback = &echo, .access = 1 });
    api->hook_msg((command){ .triggered = true, .name = "about", .callback = &about });
    api->hook_msg((command){ .triggered = true, .name = "commands", .callback = &commands });
}

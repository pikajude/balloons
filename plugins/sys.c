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

static void cdecl(context ctx) {
    char response[1024] = { 0 };
    if (strchr(ctx.msg, '\'') != NULL) {
        strcpy(response, "syntax error");
    } else {
        char cmd[strlen(ctx.msg) + 30];
        strcpy(cmd, "echo '");
        strcat(cmd, ctx.msg);
        strcat(cmd, "' | /usr/local/bin/cdecl");
        FILE *f = popen(cmd, "r");
        fread(response, 1023, 1, f);
        pclose(f);
    }
    dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), response);
}

static void can(context ctx) {
    char uname[128], cmd[128];
    unsigned char uaccess, caccess;
    if (sscanf(ctx.msg, "%s use %[^? ]?", uname, cmd) < 2) {
        dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), "I don't know.");
    } else {
        if ((uname[0] == 'I' || uname[0] == 'i') && uname[1] == '\0') strcpy(uname, ctx.sender);
        uaccess = access_get(uname);
        caccess = access_get_cmd(api->events, cmd);
        dsendmsg(ctx.damn, pkt_roomname(ctx.pkt), uaccess >= caccess ? "Yes." : "No.");
    }
}

void balloons_init(_api *a) {
    api = a;
    api->hook_msg((command){ .callback = &trigcheck });
    api->hook_msg((command){ .callback = &botcheck });
    pinghookid = api->hook_msg((command){ .triggered = true, .name = "ping", .callback = &ping });
    api->hook_msg((command){ .triggered = true, .name = "echo", .callback = &echo, .access = 1 });
    api->hook_msg((command){ .triggered = true, .name = "about", .callback = &about });
    api->hook_msg((command){ .triggered = true, .name = "commands", .callback = &commands });
    api->hook_msg((command){ .triggered = true, .name = "cdecl", .callback = &cdecl });
    api->hook_msg((command){ .triggered = true, .name = "can", .callback = &can });
}

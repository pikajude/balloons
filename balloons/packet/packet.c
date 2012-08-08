#include "packet.h"

static size_t _parse_cmd(packet *p, const char *str) {
    size_t idx = strcspn(str, " \n");
    p->command = malloc(idx + 1);
    if (p->command == NULL)
        HANDLE_ERR("Unable to allocate memory for p->command");
    strncpy(p->command, str, idx);
    p->command[idx] = 0;
    return idx;
}

static size_t _parse_subcmd(packet *p, const char *str) {
    if (*str != ' ')
        return 0;
    str++;
    size_t idx = strcspn(str, "\n") + 1;
    p->subcommand = malloc(idx);
    if (p->subcommand == NULL)
        HANDLE_ERR("Unable to allocate memory for p->subcommand");
    strncpy(p->subcommand, str, idx - 1);
    p->subcommand[idx - 1] = 0;
    return idx + 1;
}

static size_t _parse_body(packet *p, const char *str) {
    size_t len = strlen(str);
    p->body = malloc(len + 1);
    if (p->body == NULL)
        HANDLE_ERR("Unable to allocate memory for p->body");
    strncpy(p->body, str, len);
    p->body[len] = 0;
    return 0; // doesn't matter, because nothing is called after this
}

static size_t _parse_argpair(packet *p, const char *str) {
    // at the end of the argument pairs
    if (*str == 0 || *str == '\n') return 0;
    
    // get the key
    size_t idx = strcspn(str, "=\n");
    if (str[idx] == '\n') return idx + 1;
    char *key = malloc(idx + 1);
    if (key == NULL)
        HANDLE_ERR("Unable to allocate memory for key");
    strncpy(key, str, idx);
    key[idx] = 0;
    str += (idx + 1);
    
    // get the value!
    size_t idx_n = strcspn(str, "\n");
    char *value = malloc(idx_n + 1);
    if (value == NULL)
        HANDLE_ERR("Unable to allocate memory for value");
    strncpy(value, str, idx_n);
    value[idx_n] = 0;
    
    // make the argument pair
    if (p->args == NULL)
        p->args = al_make_pair(key, value);
    else
        al_set(p->args, key, value);
    
    // consumed key, value, '=' and '\n'
    return idx + idx_n + 2;
}

packet *packet_parse(const char *str, int skip_newline) {
    packet *p = calloc(1, sizeof *p);
    if (p == NULL)
        HANDLE_ERR("Unable to allocate packet");
    str += _parse_cmd(p, str);
    str += _parse_subcmd(p, str);
    
    if (*str != '\n' && *str != 0) {
        size_t s;
        if (skip_newline) {
            s = _parse_argpair(p, str);
            str += (s + 1); // eat the extra newline
        }
        while ((s = _parse_argpair(p, str)) > 0)
            str += s;
    }
    
    if (*str == '\n')
        _parse_body(p, ++str);
    
    return p;
}

void pkt_free(packet *p) {
    if (p->args != NULL)
        al_free(p->args);
    free(p->command);
    free(p->subcommand);
    free(p->body);
    free(p);
}

void pkt_print(packet *p) {
    if (p->command != NULL)
        printf("command = %s\n", p->command);
    
    if (p->subcommand != NULL)
        printf("subcommand = %s\n", p->subcommand);
    
    al_print(p->args);
    
    if (p->body != NULL && strlen(p->body) > 0)
        printf("body = %s", p->body);
}
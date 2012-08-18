#include "packet.h"

static size_t _parse_cmd(packet *p, const wchar_t *str) {
    size_t idx = wcscspn(str, L" \n");
    p->command = calloc(1, sizeof(wchar_t) * (idx + 1));
    if (p->command == NULL)
        HANDLE_ERR("Unable to allocate memory for p->command");
    wcsncpy(p->command, str, idx);
    return idx;
}

static size_t _parse_subcmd(packet *p, const wchar_t *str) {
    if (*str != ' ')
        return 0;
    str++;
    size_t idx = wcscspn(str, L"\n") + 1;
    p->subcommand = calloc(1, sizeof(wchar_t) * idx);
    if (p->subcommand == NULL)
        HANDLE_ERR("Unable to allocate memory for p->subcommand");
    wcsncpy(p->subcommand, str, idx - 1);
    return idx + 1;
}

static size_t _parse_body(packet *p, const wchar_t *str) {
    size_t len = wcslen(str);
    p->body = calloc(1, sizeof(wchar_t) * (len + 1));
    if (p->body == NULL)
        HANDLE_ERR("Unable to allocate memory for p->body");
    wcsncpy(p->body, str, len);
    return 0; // doesn't matter, because nothing is called after this
}

static size_t _parse_argpair(packet *p, const wchar_t *str) {
    // at the end of the argument pairs
    if (*str == 0 || *str == '\n') return 0;
    
    // get the key
    size_t idx = wcscspn(str, L"=\n");
    if (str[idx] == '\n') return idx + 1;
    wchar_t *key = calloc(1, sizeof(wchar_t) * (idx + 1));
    if (key == NULL)
        HANDLE_ERR("Unable to allocate memory for key");
    wcsncpy(key, str, idx);
    str += (idx + 1);
    
    // get the value!
    size_t idx_n = wcscspn(str, L"\n");
    wchar_t *value = calloc(1, sizeof(wchar_t) * (idx_n + 1));
    if (value == NULL)
        HANDLE_ERR("Unable to allocate memory for value");
    wcsncpy(value, str, idx_n);
    
    // make the argument pair
    if (p->args == NULL)
        p->args = al_make_pair(key, value);
    else
        al_set(p->args, key, value);
    
    // consumed key, value, '=' and '\n'
    return idx + idx_n + 2;
}

packet *packet_parse(const wchar_t *str, int skip_newline) {
    packet *p = calloc(1, sizeof(packet));
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
        wprintf(L"command = %ls\n", p->command);
    
    if (p->subcommand != NULL)
        wprintf(L"subcommand = %ls\n", p->subcommand);
    
    al_print(p->args);
    
    if (p->body != NULL && wcslen(p->body) > 0)
        wprintf(L"body = %ls", p->body);
}

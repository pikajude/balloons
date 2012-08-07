#include "tablumps.h"

static char *simple_lumps[23] = {
    "b", "i", "u", "s", "sup", "sub",
    "code", "br", "ul", "ol", "li", "p",
    "/b", "/i", "/u", "/s", "/sup", "/sub",
    "/code", "/ul", "/ol", "/li", "/p"
};

static void remove_simple(char *str) {
    char match[8], *b;
    for(int i = 0; i < 23; i++) {
        memset(match, 8, 0);
        sprintf(match, "&%s\t", simple_lumps[i]);
        while((b = strstr(str, match)) != NULL) {
            b[0] = '<';
            b[strlen(match) - 1] = '>';
        }
    }
}

void delump(char *str) {
    remove_simple(str);
}

#include "tablumps.h"

static char *simple_lumps[] = {
    "b", "i", "u", "s", "sup", "sub",
    "code", "br", "ul", "ol", "li", "p",
    "/b", "/i", "/u", "/s", "/sup", "/sub",
    "/code", "/ul", "/ol", "/li", "/p",
    "bcode", "/bcode", "/abbr", "/a"
};

static lump complex_lumps[] = {
    { "&a\t",     "<a href='%' title='%'>", 2, {1, 2} },
    { "&acro\t",  "<acronym title='%'>",    1, {1}    },
    { "&/acro\t", "</acronym>",             0, {}     },
    { "&abbr\t",  "<abbr title='%'>",       1, {1}    },
    { "&emote\t", "%",                      5, {1}    },
    { "&dev\t",   ":dev%:",                 2, {2}    },
};

static void remove_simple(char *str) {
    char match[9], *b;
    for(unsigned long i = 0; i < sizeof(simple_lumps)/sizeof(char *); i++) {
        memset(match, 9, 0);
        sprintf(match, "&%s\t", simple_lumps[i]);
        while((b = strstr(str, match)) != NULL) {
            b[0] = '<';
            b[strlen(match) - 1] = '>';
        }
    }
}

static size_t countchr(char *s, char c) {
    size_t accum = 0;
    for (size_t i = 0; i < strlen(s); i++) {
        if (s[i] == c) accum++;
    }
    return accum;
}

static inline bool contains(unsigned char h[3], unsigned char n) {
    return h[0] == n || h[1] == n || h[2] == n;
}

static int *indexes(char *s, char c) {
    size_t ddx = 0;
    int *dexes = malloc(4 * sizeof(int));
    memset(dexes, -1, 4);
    for(int i = 0; i < (int)strlen(s); i++) {
        if(s[i] == c) {
            dexes[ddx] = i;
            ddx++;
        }
    }
    dexes[ddx] = (int)strlen(s);
    return dexes;
}

static int grouplen(unsigned char c[]) {
    for(int i = 0; i < 3; i++) {
        if(c[i] == 0) return i;
    }
    return 0;
}

static char *declump(char *s, lump l) {
    char *news;
    assert(strncmp(s, l.find, strlen(l.find)) == 0);
    bool use_matches = false;
    char *matches[7] = {};
    long idx_s, idx_m = 0, matchlen = 0, fullmatchlen = 0;
    if(l.arity > 0) {
        idx_s = strchr(s, '\t') - s + 1;
        for(unsigned char i = 1; i <= l.arity; i++) {
            idx_m = 0;
            matches[i - 1] = calloc(1, 512);
            use_matches = true;
            while(s[idx_s] != '\t') {
                matches[i - 1][idx_m] = s[idx_s];
                idx_m++;
                idx_s++;
            }
            if(contains(l.groups, i))
                matchlen += idx_m;
            fullmatchlen += idx_m + 1;
            idx_s++;
        }
    }
    // length of section that will be "found"
    int f_len = (int)strlen(l.find) + (int)fullmatchlen;
    // length of section that will be "replaced"
    int r_len = (int)strlen(l.repl) - (int)countchr(l.repl, '%') + (int)matchlen;
    int diff = r_len - f_len;
    
    int *rdexes = indexes(l.repl, '%');
    int *sdexes = indexes(l.repl, '%');
    int glen = grouplen(&l.groups[0]);
    size_t curmatchlen;
    
    if(diff != 0) {
        news = calloc(1, (size_t)((int)strlen(s) + diff));
        if(news == NULL) {
            printf("news is null\n");
            exit(0);
        }
        if(diff > 0)
            strcpy(news + diff, s);
        if(rdexes[0] != -1) {
            memcpy(news, l.repl, rdexes[0]);
            for(int j = 0; j < glen; j++) {
                assert(matches[l.groups[j] - 1] != NULL);
                curmatchlen = strlen(matches[l.groups[j] - 1]);
                memcpy(news + sdexes[j], matches[l.groups[j] - 1], curmatchlen);
                memcpy(news + sdexes[j] + curmatchlen, l.repl + rdexes[j] + 1, rdexes[j + 1] - rdexes[j] - 1);
                for(int q = 0; q < glen; q++)
                    sdexes[q] += curmatchlen - 1;
                printf("");
            }
        }
        if(diff < 0)
            strcat(news, s + f_len);
    } else {
        news = malloc(strlen(s));
        strcpy(news, s);
    }
    for(unsigned char k = 0; k < l.arity * use_matches; k++) {
        free(matches[k]);
    }
    free(sdexes);
    free(rdexes);
    return news;
}

static char *remove_complex(char *str) {
    char *b;
    for(unsigned long i = 0; i < sizeof(complex_lumps)/sizeof(lump); i++) {
        while((b = strstr(str, complex_lumps[i].find)) != NULL) {
            char *fix = declump(b, complex_lumps[i]);
            long offset = b - str;
            str = realloc(str, (size_t)offset + strlen(fix) + 1);
            strcpy(str + offset, fix);
            free(fix);
        }
    }
    return str;
}

char *delump(char *str) {
    remove_simple(str);
    return remove_complex(str);
}

#include "tablumps.h"

static char *simple_lumps[] = {
    "b", "i", "u", "s", "sup", "sub",
    "code", "br", "ul", "ol", "li", "p",
    "/b", "/i", "/u", "/s", "/sup", "/sub",
    "/code", "/ul", "/ol", "/li", "/p",
    "bcode", "/bcode", "/abbr", "/a", "/iframe"
};

static lump complex_lumps[] = {
    { L"&a\t",      L"<a href='%' title='%'>",                  2, {1, 2}    },
    { L"&acro\t",   L"<acronym title='%'>",                     1, {1}       },
    { L"&/acro\t",  L"</acronym>",                              0, {}        },
    { L"&abbr\t",   L"<abbr title='%'>",                        1, {1}       },
    { L"&emote\t",  L"%",                                       5, {1}       },
    { L"&dev\t",    L":dev%:",                                  2, {2}       },
    { L"&link\t",   L"% (%)",                                   3, {1, 2}    },
    { L"&avatar\t", L":icon%:",                                 2, {1}       },
    { L"&thumb\t",  L":thumb%:",                                6, {1}       },
    { L"&img\t",    L"<img src='%' alt='%' title='%' />",       3, {1, 2, 3} },
    { L"&iframe\t", L"<iframe src='%' width='%' height='%' />", 4, {1, 2, 3} },
};

static void remove_simple(wchar_t *str) {
    wchar_t match[10], *b;
    for(unsigned long i = 0; i < sizeof(simple_lumps)/sizeof(char *); i++) {
        wmemset(match, 0, 10);
        swprintf(match, 10, L"&%s\t", simple_lumps[i]);
        while((b = wcsstr(str, match)) != NULL) {
            wmemset(b, L'<', 1);
            b[wcslen(match) - 1] = L'>';
        }
    }
}

static size_t countchr(wchar_t *s, wchar_t c) {
    size_t accum = 0;
    for (size_t i = 0; i < wcslen(s); i++) {
        if (s[i] == c) accum++;
    }
    return accum;
}

static inline bool contains(unsigned char h[3], unsigned char n) {
    return h[0] == n || h[1] == n || h[2] == n;
}

static size_t *indexes(wchar_t *s, wchar_t c) {
    size_t ddx = 0;
    size_t *dexes = malloc(4 * sizeof(size_t));
    memset(dexes, -1, 4);
    for(size_t i = 0; i < wcslen(s); i++) {
        if(s[i] == c) {
            dexes[ddx] = i;
            ddx++;
        }
    }
    dexes[ddx] = wcslen(s);
    return dexes;
}

static int grouplen(unsigned char c[]) {
    for(int i = 0; i < 3; i++) {
        if(c[i] == 0) return i;
    }
    return 0;
}

static wchar_t *declump(wchar_t *s, lump l) {
    wchar_t *news;
    assert(wcsncmp(s, l.find, wcslen(l.find)) == 0);
    bool use_matches = false;
    wchar_t *matches[7] = {};
    long idx_s, idx_m = 0, matchlen = 0, fullmatchlen = 0;
    if(l.arity > 0) {
        idx_s = wcschr(s, '\t') - s + 1;
        for(unsigned char i = 1; i <= l.arity; i++) {
            idx_m = 0;
            matches[i - 1] = calloc(1, sizeof(wchar_t) * 512);
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
            // fix for variadic arguments in link
            if(i == 2 && wcsncmp(matches[1], L"&", 2) == 0 && wcsncmp(s, L"&link\t", 6) == 0) {
                wcscpy(matches[1], L"[link]");
                matchlen += 5;
                break;
            }
        }
    }
    // length of section that will be "found"
    int f_len = (int)wcslen(l.find) + (int)fullmatchlen;
    // length of section that will be "replaced"
    int r_len = (int)wcslen(l.repl) - (int)countchr(l.repl, L'%') + (int)matchlen;
    int diff = r_len - f_len;
    
    size_t *rdexes = indexes(l.repl, '%');
    size_t *sdexes = indexes(l.repl, '%');
    int glen = grouplen(&l.groups[0]);
    size_t curmatchlen;
    
    news = calloc(1, sizeof(wchar_t) * (size_t)((int)wcslen(s) + diff + 1));
    if(diff > 0)
        wcscpy(news + diff, s);
    if(rdexes[0] != (size_t)-1) {
        wmemcpy(news, l.repl, rdexes[0]);
        for(int j = 0; j < glen; j++) {
            assert(matches[l.groups[j] - 1] != NULL);
            curmatchlen = wcslen(matches[l.groups[j] - 1]);
            wmemcpy(news + sdexes[j], matches[l.groups[j] - 1], curmatchlen);
            wmemcpy(news + sdexes[j] + curmatchlen, l.repl + rdexes[j] + 1, rdexes[j + 1] - rdexes[j] - 1);
            for(int q = 0; q < glen; q++)
                sdexes[q] += curmatchlen - 1;
        }
    }
    if(diff < 0)
        wcscat(news, s + f_len);
    for(unsigned char k = 0; k < l.arity * use_matches; k++) {
        free(matches[k]);
    }
    free(sdexes);
    free(rdexes);
    return news;
}

static wchar_t *remove_complex(wchar_t *str) {
    wchar_t *b, *fix;
    for(unsigned long i = 0; i < sizeof(complex_lumps)/sizeof(lump); i++) {
        while((b = wcsstr(str, complex_lumps[i].find)) != NULL) {
            fix = declump(b, complex_lumps[i]);
            long offset = b - str;
            str = realloc(str, sizeof(wchar_t) * ((size_t)offset + wcslen(fix) + 1));
            wcscpy(str + offset, fix);
            free(fix);
        }
    }
    return str;
}

wchar_t *delump(wchar_t *str) {
    remove_simple(str);
    return remove_complex(str);
}

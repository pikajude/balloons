#include "setup.h"

static void nuke_settings(void) {
    wchar_t *fname = settings_filename();
    char *afname = calloc(1, wcslen(fname) * 2);
    wcstombs(afname, fname, wcslen(fname) * 2);
    fclose(fopen(afname, "w"));
    free(fname);
    free(afname);
    settings_load(true);
}

static int getfirstchar(void) {
    int c = getchar();
    if (c == EOF) {
        if (feof(stdin)) {
            printf("Lost user.\n");
            exit(EXIT_FAILURE);
        } else
            return 0;
    } else {
        for (;;) {
            int _unused = getchar();
            if (_unused == '\n' || _unused == EOF)
                break;
        }
        return c;
    }
}

static void setup_get_token(void) {
    token_get_access_all();
    wprintf(L"You authorized %ls. Is that the right account? (y/n) ", setting_get(BKEY_USERNAME));
    if (getfirstchar() != 'y') {
        nuke_settings();
        return setup_get_token();
    }
}

static void setup_get_trigger(void) {
    if(setting_get(BKEY_TRIGGER) != NULL) return;
    wchar_t *trigger = calloc(1, sizeof(wchar_t) * (wcslen(default_trigger) + 1));
    char *holder = calloc(1, 1);
    if (trigger == NULL)
        HANDLE_ERR("Unable to allocate trigger space");
    wcscpy(trigger, default_trigger);
    size_t size = 1;
    wprintf(L"Enter a trigger for the bot [%ls]: ", default_trigger);
    getline(&holder, &size, stdin);
    mbstowcs(trigger, holder, strlen(holder));
    
    wchar_t *nl = wcsrchr(trigger, '\n');
    nl[0] = 0;
    
    if (wcslen(trigger) == 0) {
        setting_store(BKEY_TRIGGER, default_trigger);
        return;
    }
    wprintf(L"You chose %ls for the trigger. Is that okay? (y/n) ", trigger);
    if (getfirstchar() != 'y')
        return setup_get_trigger();
    else
        setting_store(BKEY_TRIGGER, trigger);
}

static void setup_get_extpath(void) {
    if(setting_get(BKEY_EXTENSIONS_DIR) != NULL) return;
    char *holder = malloc(1);
    size_t size = 0;
    wchar_t *curpath = settings_dirname();
    wchar_t *extname = calloc(1, sizeof(wchar_t) * (wcslen(curpath) + 9));
    if (extname == NULL)
        HANDLE_ERR("Unable to allocate space for extensions path");
    wcscat(extname, curpath);
    wcscat(extname, L"/plugins");
    wprintf(L"Enter the (full!) path to the directory where plugins will be stored [%ls]: ", extname);
    getline(&holder, &size, stdin);
    
    if (*holder == '\n') {
        char *aextname = calloc(1, wcslen(extname) * 2);
        wcstombs(aextname, extname, wcslen(extname) * 2);
        if (mkdir(aextname, 0755) < 0) {
            perror("Couldn't create the plugin directory.");
            nuke_settings();
            exit(EXIT_FAILURE);
        }
        setting_store(BKEY_EXTENSIONS_DIR, extname);
        free(extname);
        free(aextname);
        return;
    }
    
    char *nl = strrchr(holder, '\n');
    nl[0] = '\0';
    
    wchar_t *path = malloc(sizeof(wchar_t) * (strlen(holder) + 1));
    mbstowcs(path, holder, strlen(holder));
    
    printf("You chose %s as the plugins directory. Is that okay? (y/n) ", holder);
    free(extname);
    if (getfirstchar() != 'y') {
        free(path);
        return setup_get_extpath();
    } else {
        if (mkdir(holder, 0755) < 0) {
            perror("Couldn't create the plugin directory");
            printf("Assuming that means it already exists. Continuing.\n");
        }
        setting_store(BKEY_EXTENSIONS_DIR, path);
    }
}

static void setup_get_autojoin(void) {
    if(setting_get(BKEY_AUTOJOIN) != NULL) return;
    wchar_t *autojoin;
    char *holder = malloc(1);
    size_t size = 0;
    printf("Enter the default chatrooms for the bot to join, separated by spaces ONLY, '#' character optional: ");
    getline(&holder, &size, stdin);
    if(*holder == '\n')
        return setup_get_autojoin();
    
    char *nl = strrchr(holder, '\n');
    nl[0] = '\0';
    
    printf("You chose '%s' for the autojoin list. Is that okay? (y/n) ", holder);
    if(getfirstchar() != 'y') {
        free(holder);
        return setup_get_autojoin();
    } else {
        autojoin = calloc(1, sizeof(wchar_t) * (strlen(holder) + 1));
        mbstowcs(autojoin, holder, strlen(holder));
        setting_store(BKEY_AUTOJOIN, autojoin);
        free(holder);
    }
}

static void setup_get_owner(void) {
    char *owner = calloc(1, 1);
    size_t size = 0;
    printf("Enter the username of the bot's owner (probably yours): ");
    getline(&owner, &size, stdin);
    if(*owner == '\n')
        return setup_get_owner();
    
    char *nl = strrchr(owner, '\n');
    nl[0] = '\0';
    
    printf("You chose '%s' as the bot's owner. Is that okay? (y/n) ", owner);
    if(getfirstchar() != 'y') {
        free(owner);
        return setup_get_owner();
    } else {
        wchar_t *sname = calloc(1, sizeof(wchar_t) * (8 + strlen(owner)));
        swprintf(sname, 8 + strlen(owner), L"access.%s", owner);
        setting_store(sname, L"254");
        free(owner);
        free(sname);
    }
}

void runsetup(void) {
    if (setting_get(BKEY_OAUTHCODE) != NULL)
        return;
    puts("Thanks for choosing balloons! You rock.");
    setup_get_token();
    setup_get_trigger();
    setup_get_extpath();
    setup_get_autojoin();
    setup_get_owner();
    puts("All set!");
}

#include "setup.h"

static void nuke_settings(void) {
    fclose(fopen(settings_filename(), "w"));
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
    printf("You authorized %s. Is that the right account? (y/n) ", setting_get(BKEY_USERNAME));
    if (getfirstchar() != 'y') {
        nuke_settings();
        return setup_get_token();
    }
}

static void setup_get_trigger(void) {
    char *trigger = calloc(1, strlen(default_trigger) + 1);
    if (trigger == NULL)
        HANDLE_ERR("Unable to allocate trigger space");
    strcpy(trigger, default_trigger);
    size_t size = 1;
    printf("Enter a trigger for the bot [%s]: ", default_trigger);
    getline(&trigger, &size, stdin);
    for (size_t i = 0; i < size; i++)
        if (trigger[i] == '\n') {
            trigger[i] = '\0';
            break;
        }
    if (strlen(trigger) == 0) {
        setting_store(BKEY_TRIGGER, default_trigger);
        return;
    }
    printf("You chose %s for the trigger. Is that okay? (y/n) ", trigger);
    if (getfirstchar() != 'y')
        return setup_get_trigger();
    else
        setting_store(BKEY_TRIGGER, trigger);
}

static void setup_get_extpath(void) {
    char *path = calloc(1, 1);
    if (path == NULL)
        HANDLE_ERR("Unable to allocate space for path");
    size_t size = 0;
    char *curpath = settings_dirname();
    char *extname = calloc(1, strlen(curpath) + 9);
    if (extname == NULL)
        HANDLE_ERR("Unable to allocate space for extensions path");
    strcat(extname, curpath);
    strcat(extname, "/plugins");
    printf("Enter the (full!) path to the directory where plugins will be stored [%s]: ", extname);
    getline(&path, &size, stdin);
    
    if (*path == '\n') {
        if (mkdir(extname, 0755) < 0) {
            perror("Couldn't create the plugin directory.");
            nuke_settings();
            exit(EXIT_FAILURE);
        }
        setting_store(BKEY_EXTENSIONS_DIR, extname);
        free(extname);
        return;
    }
    
    char *nl = strrchr(path, '\n');
    nl[0] = '\0';
    printf("You chose %s as the plugins directory. Is that okay? (y/n) ", path);
    free(extname);
    if (getfirstchar() != 'y')
        return setup_get_extpath();
    else {
        if (mkdir(path, 0755) < 0) {
            perror("Couldn't create the plugin directory");
            printf("Assuming that means it already exists. Continuing.\n");
        }
        setting_store(BKEY_EXTENSIONS_DIR, path);
    }
}

static void setup_get_autojoin(void) {
    char *autojoin = calloc(1, 1);
    size_t size = 0;
    printf("Enter the default chatrooms for the bot to join, separated by spaces ONLY, '#' character optional: ");
    getline(&autojoin, &size, stdin);
    if(*autojoin == '\n')
        return setup_get_autojoin();
    
    char *nl = strrchr(autojoin, '\n');
    nl[0] = '\0';
    
    printf("You chose '%s' for the autojoin list. Is that okay? (y/n) ", autojoin);
    if(getfirstchar() != 'y') {
        free(autojoin);
        return setup_get_autojoin();
    } else {
        setting_store(BKEY_AUTOJOIN, autojoin);
        free(autojoin);
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
        char *sname = calloc(1, 8 + strlen(owner));
        strcpy(sname, "access.");
        strcat(sname, owner);
        setting_store(sname, "254");
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

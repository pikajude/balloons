#include "setup.h"

static void nuke_settings(void) {
    fclose(fopen(settings_filename(), "w"));
    settings_load(true);
}

static char getfirstchar(void) {
    char c = (char)getchar();
    while (getchar() != '\n');
    return c;
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
    size_t size = 0;
    char *curpath = settings_dirname();
    char *extname = calloc(1, strlen(curpath) + 9);
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
        return;
    }
    
    printf("You chose %s as the plugins directory. Is that okay? (y/n) ", path);
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

void runsetup(void) {
    if (setting_get(BKEY_OAUTHCODE) != NULL)
        return;
    puts("Thanks for choosing balloons! You rock.");
    setup_get_token();
    setup_get_trigger();
    setup_get_extpath();
    puts("All set!");
}

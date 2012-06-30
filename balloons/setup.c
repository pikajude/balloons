#include "setup.h"

void runsetup(void) {
    if (setting_get(BKEY_OAUTHCODE) != NULL)
        return;
    puts("Hi!");
    exit(0);
}

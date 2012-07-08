#pragma once

#define BVERSION            "0.0"
#define BINIT_FUNCTION      "balloons_init"
#define BKEY_USERNAME       "_username"
#define BKEY_OAUTHCODE      "_oauthtoken"
#define BKEY_OAUTHRTOKEN    "_oauthrtoken"
#define BKEY_TRIGGER        "_trigger"
#define BKEY_EXTENSIONS_DIR "_extdir"

#define zero(a,b) memset(a,0,b)
#define false 0
#define true 1

#define handle_err(str) do { perror(str); exit(EXIT_FAILURE); } while (0)

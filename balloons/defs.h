#pragma once

#define BVERSION            "0.0"
#define BINIT_FUNCTION      "balloons_init"
#define BKEY_USERNAME       "_username"
#define BKEY_OAUTHCODE      "_oauthtoken"
#define BKEY_OAUTHRTOKEN    "_oauthrtoken"
#define BKEY_TRIGGER        "_trigger"
#define BKEY_EXTENSIONS_DIR "_extdir"
#define BCMDLEN_MAX         16
#define CMD_TIMEOUT         10

#define zero(a,b) memset(a,0,b)

#define HANDLE_ERR(str) do { perror(str); exit(EXIT_FAILURE); } while (0)

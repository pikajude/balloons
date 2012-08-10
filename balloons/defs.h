#pragma once

#define BVERSION            L"0.0"
#define BINIT_FUNCTION      "balloons_init"
#define BKEY_USERNAME       L"username"
#define BKEY_OAUTHCODE      L"_oauthtoken"
#define BKEY_OAUTHRTOKEN    L"_oauthrtoken"
#define BKEY_TRIGGER        L"trigger"
#define BKEY_EXTENSIONS_DIR L"_extdir"
#define BKEY_AUTOJOIN       L"autojoin"
#define BCMDLEN_MAX         16
#define CMD_TIMEOUT         10

#define HANDLE_ERR(str) do { perror(str); exit(EXIT_FAILURE); } while (0)

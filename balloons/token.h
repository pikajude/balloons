#pragma once

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "settings.h"
#include "defs.h"

#define CLIENT_ID L"209"
#define CLIENT_SECRET L"85d3430961516f158846b776462db13f"

wchar_t *token_get_code(void);
wchar_t *token_get_access(wchar_t*, int);
wchar_t *token_get_access_all(void);
wchar_t *token_get_damn(wchar_t*);

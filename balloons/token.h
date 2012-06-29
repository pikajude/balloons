#pragma once

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "settings.h"
#include "defs.h"

#define CLIENT_ID "209"
#define CLIENT_SECRET "85d3430961516f158846b776462db13f"

char *token_get_code(void);
char *token_get_access(char*, int);
char *token_get_access_all(void);
char *token_get_damn(char*);

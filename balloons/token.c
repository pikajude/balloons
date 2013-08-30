#include "token.h"

static char *response(void) {
    char date[30] = { 0 };
    char *resp = calloc(1, 175);
    if (resp == NULL)
        HANDLE_ERR("Unable to allocate response room");
    time_t rawtime;
    time(&rawtime);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S GMT", localtime(&rawtime));
    strcat(resp, "HTTP/1.0 200 OK\r\nDate: ");
    strcat(resp, date);
    strcat(resp, "\r\nContent-Type: text/plain\r\nContent-Length: 52\r\n\r\nThanks for using balloons! You can close the window.\r\n");
    return resp;
}

static wchar_t *extractJSON(char *json, char *key) {
    size_t keylen = strlen(key), idx = 0, jsonlen = strlen(json);
    while(strncmp(json + idx, key, keylen) != 0) {
        idx++;
        if(idx > jsonlen)
            return NULL;
    }
    char *k = json + idx + keylen + 3;
    char *nl = strchr(k, '"');
    char *value = calloc(1, (size_t)nl - (size_t)k + 1);
    strncpy(value, k, nl - k);
    wchar_t *wval = calloc(1, sizeof(wchar_t) * (strlen(value) + 1));
    mbstowcs(wval, value, strlen(value));
    free(value);
    return wval;
}

static size_t write_callback(void *data, size_t size1, size_t size2, void *extant_data) {
    size_t newsize = size1 * size2;
    char **str = extant_data;
    unsigned long len = strlen(*str);
    *str = realloc(*str, len + newsize + 1);
    if (*str == NULL)
        HANDLE_ERR("Unable to reallocate string in write_callback");
    strncat(*str, data, newsize);
    return newsize;
}

static char *curl_request(char *url, arglist *params) {
    CURL *curl;
    CURLcode response_code;
    char *buffer = malloc(1);
    if (buffer == NULL)
        HANDLE_ERR("Unable to allocate memory for buffer");

    wchar_t *full_url = malloc(sizeof(wchar_t) * (strlen(url) + 2));
    if (full_url == NULL)
        HANDLE_ERR("Unable to allocate memory for full URL");
    swprintf(full_url, strlen(url) + 2, L"%s?", url);
    while (params != NULL) {
        full_url = realloc(full_url, sizeof(wchar_t) * (wcslen(full_url) + 3 + wcslen(params->key) + wcslen(params->value)));
        if (full_url == NULL)
            HANDLE_ERR("Unable to resize full_url");
        wcscat(full_url, params->key);
        wcscat(full_url, L"=");
        wcscat(full_url, params->value);
        if (params->next != NULL) wcscat(full_url, L"&");
        params = params->next;
    }

    curl_global_init(CURL_GLOBAL_ALL);

    char *ascii_url = calloc(1, wcslen(full_url) * 4);
    wcstombs(ascii_url, full_url, wcslen(full_url) * 4);

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, ascii_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    response_code = curl_easy_perform(curl);
    if (response_code != CURLE_OK) {
        printf("Wowzer! curl failed: %s\n", curl_easy_strerror(response_code));
        exit(EXIT_FAILURE);
    }

    free(full_url);
    free(ascii_url);

    curl_free(curl);

    curl_global_cleanup();

    return buffer;
}

static void token_whoami(wchar_t *accesstoken) {
    arglist *params = al_make_pair(L"access_token", accesstoken);
    char *r = curl_request("https://www.deviantart.com/api/draft15/user/whoami", params);
    al_free(params);
    wchar_t *success = extractJSON(r, "status");
    if (success != NULL && wcscmp(success, L"error") == 0)
        HANDLE_ERR("Bad token!");
    free(success);
    wchar_t *uname = extractJSON(r, "username");
    free(r);
    setting_store(BKEY_USERNAME, uname);
    free(uname);
}

wchar_t *token_get_code(void) {
    wchar_t *curtok = setting_get(BKEY_OAUTHCODE);
    if (curtok != NULL)
        return curtok;
    char buffer[300];
    char *code = malloc(11);
    if (code == NULL)
        HANDLE_ERR("Unable to allocate memory for OAuth code");
    struct sockaddr_in addr, client_addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof addr) != 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }
    listen(sockfd, 5);

    printf("In whatever browser you use, log into the account for the bot you'll be using.\nThen authorize balloons at http://goo.gl/S3cRb\n");

    socklen_t client_len = sizeof client_addr;
    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    recv(clientfd, &buffer, sizeof(buffer) - 1, 0);
    sscanf(buffer, "GET /?code=%s", code);

    char *resp = response();
    send(clientfd, resp, strlen(resp), 0);
    free(resp);
    close(clientfd);
    close(sockfd);

    wchar_t *wcode = calloc(1, sizeof(wchar_t) * strlen(code));
    mbstowcs(wcode, code, strlen(code));
    free(code);

    setting_store(BKEY_OAUTHCODE, wcode);
    return wcode;
}

wchar_t *token_get_access(wchar_t *code, int refresh) {
    arglist *params = al_make_pair(L"grant_type", refresh ? L"refresh_token" : L"authorization_code");
    al_set(params, L"client_id", CLIENT_ID);
    al_set(params, L"client_secret", CLIENT_SECRET);
    al_set(params, refresh ? L"refresh_token" : L"code", code);
    char *r = curl_request("https://www.deviantart.com/oauth2/draft15/token", params);
    al_free(params);
    wchar_t *err = extractJSON(r, "status");
    if(err == NULL) {
        printf("Uh-oh! Didn't get a valid JSON response. Got:\n%s\n", r);
        exit(1);
    }
    if (wcscmp(L"error", err) == 0) {
        wchar_t *desc = extractJSON(r, "error_description");
        wprintf(L"Error! %ls\n", desc);
        free(r);
        free(err);
        free(desc);
        exit(0);
    } else {
        free(err);
        wchar_t *rtok = extractJSON(r, "refresh_token");
        setting_store(BKEY_OAUTHRTOKEN, rtok);
        wchar_t *tok = extractJSON(r, "access_token");
        free(r);
        free(rtok);

        if (!refresh)
            token_whoami(tok);

        return tok;
    }
}

wchar_t *token_get_access_all(void) {
    int has_rtoken = setting_exists(BKEY_OAUTHRTOKEN);
    return token_get_access(has_rtoken ?
                                setting_get(BKEY_OAUTHRTOKEN) :
                                setting_exists(BKEY_OAUTHCODE) ?
                                    setting_get(BKEY_OAUTHCODE) :
                                    token_get_code(),
                            has_rtoken);
}

wchar_t *token_get_damn(wchar_t *access_token) {
    arglist *params = al_make_pair(L"access_token", access_token);
    char *r = curl_request("https://www.deviantart.com/api/draft15/user/damntoken", params);
    wchar_t *tok = extractJSON(r, "damntoken");
    free(r);
    al_free(params);
    return tok;
}

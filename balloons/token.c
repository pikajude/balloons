#include "token.h"

static char *response(void) {
    char date[30];
    char *resp = malloc(4000);
    zero(resp, 4000);
    time_t rawtime;
    time(&rawtime);
    strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", localtime(&rawtime));
    strcat(resp, "HTTP/1.0 200 OK\r\nDate: ");
    strcat(resp, date);
    strcat(resp, "\r\nContent-Type: text/plain\r\nContent-Length: 52\r\n\r\nThanks for using balloons! You can close the window.\r\n");
    return resp;
}

static char *extractJSON(char *json, char *key) {
    char *value = malloc(512), formatstring[strlen(key) + 16];
    zero(formatstring, sizeof(formatstring));
    size_t idx = 0, jsonlen = strlen(json);
    sprintf(formatstring, "\"%s\": \"%%[^\"]\"", key);
    while (sscanf(idx+++json, formatstring, value) == 0) { if(idx > jsonlen) return NULL; }
    return value;
}

static size_t write_callback(void *data, size_t size1, size_t size2, void *extant_data) {
    size_t newsize = size1 * size2;
    char **str = (char **)extant_data;
    unsigned long len = strlen(*str);
    *str = realloc(*str, len + newsize + 1);
    strncat(*str, data, newsize);
    return newsize;
}

static char *curl_request(char *url, arglist *params) {
    CURL *curl;
    CURLcode response_code;
    char *buffer = malloc(1);
    
    char *full_url = malloc(strlen(url));
    sprintf(full_url, "%s?", url);
    while (params != NULL) {
        full_url = realloc(full_url, strlen(full_url) + 2 + strlen(params->key) + strlen(params->value));
        strcat(full_url, params->key);
        strcat(full_url, "=");
        strcat(full_url, params->value);
        if (params->next != NULL) strcat(full_url, "&");
        params = params->next;
    }
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, full_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    
    response_code = curl_easy_perform(curl);
    if (response_code != CURLE_OK) {
        printf("Wowzer! curl failed: %s\n", curl_easy_strerror(response_code));
        exit(EXIT_FAILURE);
    }
    
    curl_free(curl);
    
    curl_global_cleanup();
    
    return buffer;
}

static void token_whoami(char *accesstoken) {
    arglist *params = al_make_pair("access_token", accesstoken);
    char *r = curl_request("https://www.deviantart.com/api/draft15/user/whoami", params);
    al_free(params);
    char *uname = extractJSON(r, "username");
    free(r);
    setting_store("_username", uname);
    free(uname);
}

char *token_get_code(void) {
    char *curtok = setting_get("_oauthcode");
    if (curtok != NULL)
        return curtok;
    char buffer[300];
    char *code = malloc(11);
    struct sockaddr_in addr, client_addr;
    zero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        printf("Unable to create listening server (someone might already be using port 12345). Exiting.\n");
        exit(EXIT_FAILURE);
    }
    listen(sockfd, 5);
    
    printf("In whatever browser you use, log into the bot's account.\nThen authorize balloons at http://goo.gl/S3cRb\n");
    
    socklen_t client_len = sizeof(client_addr);
    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    read(clientfd, &buffer, sizeof(buffer) - 1);
    sscanf(buffer, "GET /?code=%s", code);
    
    char *resp = response();
    write(clientfd, resp, strlen(resp));
    close(clientfd);
    close(sockfd);
    
    setting_store("_oauthcode", code);
    return code;
}

char *token_get_access(char *code, bool refresh) {
    arglist *params = al_make_pair("grant_type", refresh ? "refresh_token" : "authorization_code");
    al_set(params, "client_id", CLIENT_ID);
    al_set(params, "client_secret", CLIENT_SECRET);
    al_set(params, refresh ? "refresh_token" : "code", code);
    char *r = curl_request("https://www.deviantart.com/oauth2/draft15/token", params);
    al_free(params);
    if (strcmp("error", extractJSON(r, "status")) == 0) {
        printf("Error! %s\n", extractJSON(r, "error_description"));
        free(r);
        return NULL;
    } else {
        setting_store("_oauthrtoken", extractJSON(r, "refresh_token"));
        char *tok = extractJSON(r, "access_token");
        free(r);
        
        if (!refresh)
            token_whoami(tok);
        
        return tok;
    }
}

char *token_get_access_all(void) {
    bool has_rtoken = setting_exists("_oauthrtoken");
    return token_get_access(has_rtoken ?
                                setting_get("_oauthrtoken") :
                                setting_exists("_oauthcode") ?
                                    setting_get("_oauthcode") :
                                    token_get_code(),
                            has_rtoken);
}

char *token_get_damn(char *access_token) {
    arglist *params = al_make_pair("access_token", access_token);
    char *r = curl_request("https://www.deviantart.com/api/draft15/user/damntoken", params);
    char *tok = extractJSON(r, "damntoken");
    free(r);
    al_free(params);
    return tok;
}
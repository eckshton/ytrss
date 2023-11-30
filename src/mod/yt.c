#include "yt.h"
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

const char* ckeys[KEY_CT] = {
    "<entry>", 
    "<title>", 
    "<published>", 
    "<id>yt:channel:"
};
const char* vkeys[KEY_CT] = {
    "<entry>", 
    "<title>", 
    "<published>", 
    "<id>yt:video:"
};
const char* ekeys[KEY_CT] = {
    "</entry>", 
    "</title>", 
    "</published>", 
    "</id>"
};

enum Key next_key(char* data) {
    enum Key match = -1;
    char* match_pos = data + strlen(data) * sizeof(char);
    for(int i = 0; i < KEY_CT; i++) {
        char* pos = strstr(data, ckeys[i]);
        if(!pos || pos > match_pos) {
            continue;
        }
        match = i;
        match_pos = pos;
    }
    return match;
}
void shift_to_key(enum Key key, char* data) {
    int n = (strstr(data, ckeys[key]) - data) + strlen(ckeys[key]);
    memmove(data, data + n, strlen(data) - n);
}
int count_key(char* data, const char* key) {
    char* pos = data;
    int res = 0;
    while((pos = strstr(pos, key))) {
        pos++;
        res++;
    }
    res++;
    return res;
}
Feed extract_head(char* data) {
    enum Key key = -1;
    int exit = 0;
    Feed res;
    bzero(&res, sizeof(Feed));

    while(!exit) {
        key = next_key(data);
        char* val_start = strstr(data, ckeys[key]) + strlen(ckeys[key]) * sizeof(char);
        int n = strstr(data, ekeys[key]) - val_start;
        switch(key) {
            case TITLE:
                strncpy(res.name, val_start, n);
                break;
            case ID:
                strncpy(res.id, val_start, n);
                break;
            case DATE:
                strncpy(res.date, val_start, n);
                break;
            case ENTRY:
                exit = 1;
        }
        shift_to_key(key, data);
    }
    res.video_ct = count_key(data, ckeys[ENTRY]);
    res.titles = malloc(sizeof(char*) * res.video_ct);
    res.dates = malloc(sizeof(char*) * res.video_ct);
    res.ids = malloc(sizeof(char*) * res.video_ct);
    return res;
}
void parse_rss(Feed* feed, char* data) {
    (*feed) = extract_head(data);
    char** go_here[4] = {NULL, feed->titles, feed->dates, feed->ids};
    for(int i = 1; i < 4; i++) {
        char* pos = data;
        for(int j = 0; j < feed->video_ct; j++) {
            pos = strstr(pos, vkeys[i]);
            pos += strlen(vkeys[i]) * sizeof(char);
            int n = strstr(pos, ekeys[i]) - pos;
            go_here[i][j] = malloc(sizeof(char) * n);
            strncpy(go_here[i][j], pos, n);
        }
    }
}
char* form_request(const char* channel_id) {
    const char* strs[3] = {
        "GET /feeds/videos.xml?channel_id=", 
        channel_id, 
        " HTTP/1.1\r\nHost: www.youtube.com\r\nConnection: close\r\n\r\n"
    };

    int len = 1;
    for(int i = 0; i < 3; i++) {
        len += strlen(strs[i]);
    }

    char* res = malloc(len);

    bzero(res, len);
    res[0] = '\0';
    for(int i = 0; i < 3; i++) {
        strcat(res, strs[i]);
    }
    return res;
}
char** extract_ids(char* data, int* channel_ct) {
    char** res = malloc(1 * sizeof(char*));
    char* nl_ptr = data;
    *channel_ct = strlen(data) % CID_LEN;

    res = realloc(res, *channel_ct * sizeof(char*));
    for(int i = 0; i < *channel_ct; i++) {
        res[i] = malloc((CID_LEN + 1) * sizeof(char));
        strncpy(res[i], data, CID_LEN);
        res[i][CID_LEN] = '\0';
        memmove(data, data + CID_LEN + 1, strlen(data) - CID_LEN);
    }

    return res;
}

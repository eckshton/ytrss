#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "mod/yt.h"
#include "mod/io.h"
#include "mod/nc.h"

typedef struct sockaddr sockaddr;

Feed* fetch_feeds(char* source_file, int* feed_ct) {
    const sockaddr* addr = (sockaddr*)form_addr("youtube.com");
    int channel_ct = 0;
    char* src = read_file(source_file);
    char** channels = extract_ids(src, &channel_ct);
    Feed* res;

    res = malloc(sizeof(Feed) * channel_ct);
    bzero(res, sizeof(Feed) * channel_ct);
    for(int i = 0; i < channel_ct; i++) {
        const int fd = init_socket();
        if(connect(fd, addr, sizeof(struct sockaddr)) == -1) {
            exit(1);
        }

        char* req = form_request(channels[i]);
        char* data = read_stream(fd, req);
        parse_rss(&res[i], data);
        (*feed_ct)++;

        shutdown(fd, SHUT_RDWR);
        close(fd);
        free(req);
    }

    free(src);
    return res;
}
char** get_feed_names(Feed* feeds, int feed_ct) {
    char** res = malloc(sizeof(char*) * feed_ct);
    for(int i = 0; i < feed_ct; i++) {
        res[i] = malloc(sizeof(char) * strlen(feeds[i].name));
        strcpy(res[i], feeds[i].name);
    }
    return res;
}
char* select_video(Feed* feeds, int feed_ct) {
    char** names = get_feed_names(feeds, feed_ct);
    int vid = -1;
    int chnl = -1;
    while(vid < 0) {
        int temp = chnl == -1 ?
            select_from_list(names, feed_ct) :
            select_from_list(feeds[chnl].titles, feeds[chnl].video_ct);
        switch(temp) {
            case -1:
                if(chnl == -1) {
                    return NULL;
                } else {
                    chnl = -1;
                    vid = -1;
                }
                continue;
            default:
                if(chnl == -1) {
                    chnl = temp;
                } else {
                    vid = temp;
                }
                continue;
        }
    }
    return feeds[chnl].ids[vid];
}
void write_feed(Feed* src, char* path) {
    FILE* file = fopen(path, "wb");
    fwrite(src, sizeof(Feed), 1, file);
    char** q[3] = {src->titles, src->dates, src->ids};
    int size[3] = {NAME_LEN, DATE_LEN, VID_LEN};
    for(int j = 0; j < 3; j++) {
        fprintf(file, "%c", '\n');
        char big[size[j]];
        for(int i = 0; i < src->video_ct; i++) {
            bzero(big, size[j] * sizeof(char));
            strcpy(big, q[j][i]);
            fwrite(&big, sizeof(char) * size[j], 1, file);
        }
    }
    fclose(file);
}
void read_feed(Feed* dst, char* path) {
    FILE* file = fopen(path, "r");
    fread(dst, sizeof(Feed), 1, file);
    dst->titles = malloc(sizeof(char*) * dst->video_ct);
    dst->dates = malloc(sizeof(char*) * dst->video_ct);
    dst->ids = malloc(sizeof(char*) * dst->video_ct);

    char** q[3] = {dst->titles, dst->dates, dst->ids};
    int size[3] = {NAME_LEN, DATE_LEN, VID_LEN};
    for(int j = 0; j < 3; j++) {
        fseek(file, 1, SEEK_CUR);
        char big[size[j]];
        for(int i = 0; i < dst->video_ct; i++) {
            bzero(big, size[j] * sizeof(char));
            fread(&big, sizeof(char) * size[j], 1, file);
            q[j][i] = malloc(sizeof(char) * (strlen(big) + 1));
            strcpy(q[j][i], big);
        }
    }
    fclose(file);
}

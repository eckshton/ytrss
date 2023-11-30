#include "reader.h"
#include "mod/nc.h"

#include <curses.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

const char cache_dir[] = "/home/eck/.cache/ytrss";
const char exec[] = "/home/eck/src/script/ytmpv.sh";

int main() {
    int feed_ct = 0;
    Feed* feeds = fetch_feeds(&feed_ct);
    init_curses();
    
    for(int i = 0; i < feed_ct; i++) {
        char* path = malloc(strlen(cache_dir) + CID_LEN + 1);
        strcpy(path, cache_dir);
        strcat(path, "/");
        strcat(path, feeds[i].id);
        write_feed(&feeds[i], path);
        free(path);
    }

    char* res_id = select_video(feeds, feed_ct);
    if(res_id == NULL) {
        endwin();
        free(feeds);
        return 0;
    }

    char cmd[250];
    strcpy(cmd, exec);
    strcat(cmd, " https://www.youtube.com/watch?v=");
    strcat(cmd, res_id);

    endwin();
    system(cmd);
    free(feeds);
    return 0;
}

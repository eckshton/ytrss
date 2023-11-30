#include "mod/yt.h"

Feed* fetch_feeds(int* feed_ct);
char* select_video(Feed* feeds, int feed_ct);
void write_feed(Feed* feed, char* path);
void read_feed(Feed* feed, char* path);

#define NAME_LEN 100
#define DATE_LEN 30
#define VID_LEN 11
#define MAX_VIDEOS 15
#define KEY_CT 4
#define CID_LEN 24

enum Key {
    ENTRY,
    TITLE,
    DATE,
    ID,
};
struct Feed {
    int video_ct;
    char name[NAME_LEN];
    char date[DATE_LEN];
    char id[CID_LEN];
    char** titles;
    char** dates;
    char** ids;
};
typedef struct Feed Feed;

void parse_rss(Feed* feed, char* data);
char* form_request(const char*);
char** extract_ids(char*, int*);

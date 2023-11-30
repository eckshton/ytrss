#include <curses.h>
#include <sys/ioctl.h>

typedef struct winsize winsize;

struct State {
    int cursor;
    int list_offset;
    winsize size;
};

typedef struct State State;

void init_curses();
int select_from_list(char** list, int LEN);

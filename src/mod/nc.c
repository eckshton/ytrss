#include <asm-generic/ioctls.h>
#include <curses.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <ncursesw/curses.h>
#include <unistd.h>
#include <wchar.h>

#include "nc.h"

int min(int a, int b) {
    if(a < b) {
        return a;
    }
    return b;
}
int max(int a, int b) {
    if(a < b) {
        return b;
    }
    return a;
}
int input(State* state, wchar_t ch, const int LEN) {
    switch(ch) {
        case 'k':
            state->cursor = max(0, state->cursor - 1);
            break;
        case 'j':
            state->cursor = min(LEN - 1, state->cursor + 1);
            break;
        case '\n':
            return 2;
        case 'q':
            return 1;
    }
    return 0;
}
winsize get_size() {
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w;
}
void init_curses() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
}
State init_state() {
    State res;
    res.cursor = 0;
    res.list_offset = 0;
    res.size = get_size();
    return res;
}
void disp_list(State* state, char** list, int len) {
    for(int i = state->list_offset; i < len && i < state->size.ws_row; i++) {
        if(i == state->cursor) {
            attrset(COLOR_PAIR(1) | A_BOLD);
        } else {
            attrset(A_NORMAL);
        }
        mvaddstr(i - state->list_offset, 0, list[i]); 
    }
}
int select_from_list(char** list, const int LEN) {
    clear();
    State state = init_state();
    while(1) {
        disp_list(&state, list, LEN);
        int in_stat = input(&state, getch(), LEN);
        switch(in_stat) {
            case 0:
                continue;
            case 2:
                return state.cursor;
            default:
                return in_stat * -1;
        }
    }
}

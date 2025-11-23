#ifndef __TERMINAL_H__
#define __TERMINAL_H__
#include <stdio.h>

static inline void clearTerminal(){
    printf("\033[H\033[J");
}

static inline void cursorPos(int x, int y){
    x++;
    y++;
    printf("\033[%d;%dH", y, x);
}

#endif
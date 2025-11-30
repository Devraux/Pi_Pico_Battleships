#ifndef __TERMINAL_H__
#define __TERMINAL_H__
#include <stdio.h>

// **************************************************
//              Color definition
// **************************************************
//              Foreground
#define BLACK               30
#define RED                 31
#define GREEN               32
#define YELLOW              33
#define BLUE                34
#define MAGENTA             35
#define CYAN                36
#define WHITE               37
#define LIGHT_BLACK         90
#define LIGHT_RED           91
#define LIGHT_GREEN         92
#define LIGHT_YELLOW        93
#define LIGHT_BLUE          94
#define LIGHT_MAGENTA       95
#define LIGHT_CYAN          96
#define LIGHT_WHITE         97

//              Background
#define BG_BLACK            40
#define BG_RED              41
#define BG_GREEN            42
#define BG_YELLOW           43
#define BG_BLUE             44
#define BG_MAGENTA          45
#define BG_CYAN             46
#define BG_WHITE            47
#define BG_LIGHT_BLACK      100
#define BG_LIGHT_RED        101
#define BG_LIGHT_GREEN      102
#define BG_LIGHT_YELLOW     103
#define BG_LIGHT_BLUE       104
#define BG_LIGHT_MAGENTA    105
#define BG_LIGHT_CYAN       106
#define BG_LIGHT_WHITE      107

#define NC                  0
// **************************************************

typedef enum {
    nc           = NC,
    Black        = BLACK,
    Red          = RED,
    Green        = GREEN,
    Yellow       = YELLOW,
    Blue         = BLUE,
    Magenta      = MAGENTA,
    Cyan         = CYAN,
    White        = WHITE,
    Light_black  = LIGHT_BLACK,
    Light_red    = LIGHT_RED,
    Light_green  = LIGHT_GREEN,
    Light_yellow = LIGHT_YELLOW,
    Light_blue   = LIGHT_BLUE,
    Light_magenta= LIGHT_MAGENTA,
    Light_cyan   = LIGHT_CYAN,
    Light_white  = LIGHT_WHITE,
} Color_t;

static inline void setCursorPosition(int x, int y){
    x++;
    y++;
    printf("\033[%d;%dH", y, x);
}

static inline void setForegroundColor(Color_t color){
    printf("\033[%dm", color);
}

static inline void setBackgroundColor(Color_t color){
    printf("\033[%dm", color+10);
}

static inline void setDefaultColor(){
    printf("\033[0m");
}

static inline void clearTerminal(){
    setDefaultColor();
    printf("\033[H\033[J");
}


#endif
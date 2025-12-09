#ifndef __ENUMS_H__
#define __ENUMS_H__

typedef enum {
    SERVER_TO_CLIENT_GET_SHIP_MAP           = 0,
    SERVER_TO_CLIENT_GET_COORDINATES        = 1,
    SERVER_TO_CLIENT_SHIP_HIT               = 2,
    SERVER_TO_CLIENT_SHIP_MISS              = 3,
    SERVER_TO_CLIENT_GAME_FINISHED          = 4,
    SERVER_TO_CLIENT_WIN                    = 5,
    SERVER_TO_CLIENT_ASK_REMATCH            = 6, // play again, same teams, if anyone answer to NO, then match HAS TO PERMANENT FINISH = CLIENT APP CLOSE
    SERVER_TO_CLIENT_WAIT_FOR_ENEMY_MOVE    = 7,
    SERVER_TO_CLIENT_DEFEAT                 = 8,

    CLIENT_TO_SERVER_COORDINATES            = 9,
    CLIENT_TO_SERVER_SHIP_MAP               = 10,
    CLIENT_TO_SERVER_IS_PLAY_CONTINUED      = 11,
} PacketType_t;

typedef enum {
    NONE        = 0,
    UNKNOW      = 1,
    LIFE        = 2,
    DEAD        = 3,
    EMPTY       = 4
} ShipStatus_t;


#endif

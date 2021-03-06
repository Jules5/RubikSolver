#ifndef ENUMS_H
#define ENUMS_H

enum RubikColor {BLACK, WHITE, RED, BLUE, ORANGE, GREEN, YELLOW, NB_COLORS};

enum RubikMoves {H,B,A,P,G,D,HI,BI,AI,PI,DI,GI,NB_MOVES};

enum RubikStates {WAIT,MOVE,MOVE_ONE,MOVE_REV,MOVE_REV_ONE};

enum FacesCube {FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT, MID_X, MID_Y, MID_Z, NB_FACES, UNKNOW_FACE};

enum TypeCube  {CENTER, EDGE, CORNER, UNKNOW_TYPE};

enum ButtonsPlayer  {PLAY_FAST_REV, PLAY_ONE_REV, PLAY, PLAY_ONE, PLAY_FAST};

enum BrowserParameters {FILE_CREATE, NB_PARAMETERS, NULL_PARAMETER};

#endif
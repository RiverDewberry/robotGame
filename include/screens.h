#ifndef SCREENS_H
#define SCREENS_H

//the return state of a screen
typedef enum ScreenState {

    //for when a screen is exited from, but the app should stay open
    LEFTSCREEN = 0,

    //for when the app closes while in a non-main screen
    EXITAPP = 1
} ScreenState;

//edits a logic board at the given path
//if none exists at that path, then one is created
ScreenState EditLogicBoard(char *lbPath);

//allows the user to select a logic board, outputs path to selected logic board
//to outPath
ScreenState SelectLogicBoard(char *outPath);

#endif

#ifndef LOGIC_BOARD_H
#define LOGIC_BOARD_H
#include <raylib.h>
#include <stdint.h>

extern char logicBoardTexturesLoaded;

// NOTE: STRUCTS

//this struct represents the data in each square on a logic board
typedef struct LogicTile {
    //the type of the tile
    int8_t type;

    //the rotation of the tile
    int8_t rotation;

    //how each tile is being powered, and from where each tile can be powered
    //the structure is as follows 0b0LLL0TTT0RRR0BBB each group of 3 bits
    //represents a side (left, top, right, bottom). Each bit represents an 
    //input. The remaining bits signal if that sides power had been updated
    //that tick.
    int16_t power;

    //power the previous tick
    int16_t prevPower;
} LogicTile;

//this struct represents a w by h grid of logic gates and circuits
typedef struct LogicBoard {
    //the amount of times the board has ticked
    uint32_t tick;

    //input to the board
    uint16_t input;

    //width of board
    uint32_t w;

    //height of board
    uint32_t h;

    //a pointer to the board
    LogicTile* board;
} LogicBoard;

//the values found in the types portion of each LogicTile
typedef enum {
    //no circuit
    NOCIR = 0,

    //straight wire
    STRWIRE = 1,

    //curved wire
    CURWIRE = 2,

    //straight double wire
    STRDWIRE = 3,

    //curved double wire
    CURDWIRE = 4,

    //logical not gate
    NOTGATE = 5,

    //buffer gate
    BUFGATE = 6,

    //logical nor gate
    NORGATE = 7,

    //logical or gate
    ORGATE = 8,

    //logical nand gate
    NANDGATE = 9,

    //logical and gate
    ANDGATE = 10,

    //logical nxor gate
    NXORGATE = 11,

    //logical xor gate
    XORGATE = 12,

    //a data latch in the off state
    OFFLATCH = 13,

    //a data latch in the on state
    ONLATCH = 14,

    //converts left and bottom single inputs to top double output
    LEFTCONVERTER = 15,

    //converts right and bottom single inputs to top double output
    RIGHTCONVERTER = 16,

    //converts left and right single inputs to top double output
    TCONVERTER = 17,

    //splits bottom input to top and left outputs
    LEFTSPLITTER = 18,

    //splits bottom input to top and right outputs
    RIGHTSPLITTER = 19,

    //splits bottom input to left and right outputs
    TSPLITTER = 20,

    //splits bottom input to all other outputs
    FULLSPLITTER = 21,

    //two single corners in a single tile
    DSINGLECORNER = 22,

    //two single wires crossing
    SINGLECROSS = 23,

    //input one, outputs the first bit of the board input
    INPUTONE = 24,

    //input two, outputs the second bit of the board input
    INPUTTWO = 25,

    //input three, outputs the third bit of the board input
    INPUTTHREE = 26,

    //input four, outputs the fourth bit of the board input
    INPUTFOUR = 27,

    //input five, outputs the fifth bit of the board input
    INPUTFIVE = 28,

    //input six, outputs the sixth bit of the board input
    INPUTSIX = 29,

    //input seven, outputs the seventh bit of the board input
    INPUTSEVEN = 30,

    //input eight, outputs the eighth bit of the board input
    INPUTEIGHT = 31,

    //input nine, outputs the nineth bit of the board input
    INPUTNINE = 32,

    //input ten, outputs the tenth bit of the board input
    INPUTTEN = 33,

    //input eleven, outputs the eleventh bit of the board input
    INPUTELEVEN = 34,

    //input twelve, outputs the twelfth bit of the board input
    INPUTTWELVE = 35,

    //input thirteen, outputs the thirteenth bit of the board input
    INPUTTHIRTEEN = 36,

    //input fourteen, outputs the fourteenth bit of the board input
    INPUTFOURTEEN = 37,

    //input fifteen, outputs the fifteenth bit of the board input
    INPUTFIFTEEN = 38,

    //input sixteen, outputs the sixteenth bit of the board input
    INPUTSIXTEEN = 39,

    //outputs one on the first tick
    STARTSIGNAL = 40

} TileTypes;

// NOTE: functions

//makes a new board of a specified width and height
LogicBoard NewLogicBoard(int w, int h);

//frees a logic board
void FreeLogicBoard(LogicBoard *lb);

//runs a logic tick on the board
void TickLogicBoard(LogicBoard* lb);

//pudates the current power of a logic board without running a tick
void UpdatePowerLogicBoard(LogicBoard* lb);

//displays the logic board onscreen
void DisplayLogicBoard(LogicBoard lb, Camera2D cam);

//loads LogicBoard textures
void LoadLogicBoardTextures();

//unloads LogicBoard textures
void UnloadLogicBoardTextures();

//gets a logicBoard from a file path relative to the executed file
LogicBoard LoadLogicBoard(const char *path);

//stores a logic board in a file at a path relative to the executed file
void StoreLogicBoard(const char *path, LogicBoard lb);

#endif

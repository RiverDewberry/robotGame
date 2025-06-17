#include "../include/logicBoard.h"
#include "../include/strutils.h"
#include "../libs/raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* NOTE: function declarations */

//gets the powered state of a tile for display
int GetPowerState(LogicTile lt);

//takes the 3 bits representing the power a side gets and swaps bits 2 and 3
int8_t RotateSidePower(int8_t sidePower);

//updates the power of the cells around a pos on a logic board
void UpdatePowerFromCell(
    LogicBoard *lb,
    uint32_t x,
    uint32_t y,
    int16_t powerOutput
);

//gets the power output of a cell in a logic board.
// WARNING: the position of the cell must be valid, since this function does
// not check before attempting to access data at that cell
int16_t GetPowerOutput(LogicBoard lb, uint32_t x, uint32_t y);

//rotates power by a given amount
int16_t rotatePower(int16_t power, int8_t rotation);
/* function declarations end*/

/* NOTE: textures */

//base tile for logic boards
Texture2D baseTile;

//single wires
Texture2D singleWires;

//double wires
Texture2D doubleWires;

//logic gates with one input
Texture2D oneInputGates;

//logic gates with two inputs
Texture2D twoInputGates;

//converters between one and two lanes
Texture2D converters;

//splits one input into multiple outputs
Texture2D splitters;

//two single wires one a single tile
Texture2D bridges;

//inputs to the logic board
Texture2D boardInputs;

//signals the starting of logic
Texture2D startSignal;

//if textures are loaded
char logicBoardTexturesLoaded;

/* textures end */

/* NOTE: exposed functions */

void LoadLogicBoardTextures()
{
    if(logicBoardTexturesLoaded)return;
    logicBoardTexturesLoaded = 1;

    char *temp = AddPath("/sprites/baseTile.png");
    baseTile = LoadTexture(temp);

    AddPathReuse("/sprites/singleWires.png", &temp);
    singleWires = LoadTexture(temp);

    AddPathReuse("/sprites/doubleWires.png", &temp);
    doubleWires = LoadTexture(temp);

    AddPathReuse("/sprites/oneInputGates.png", &temp);
    oneInputGates = LoadTexture(temp);

    AddPathReuse("/sprites/twoInputGates.png", &temp);
    twoInputGates = LoadTexture(temp);

    AddPathReuse("/sprites/converters.png", &temp);
    converters = LoadTexture(temp);

    AddPathReuse("/sprites/splitter.png", &temp);
    splitters = LoadTexture(temp);

    AddPathReuse("/sprites/bridges.png", &temp);
    bridges = LoadTexture(temp);

    AddPathReuse("/sprites/inputs.png", &temp);
    boardInputs = LoadTexture(temp);

    AddPathReuse("/sprites/signal.png", &temp);
    startSignal = LoadTexture(temp);

    free(temp);
}

void UnloadLogicBoardTextures()
{
    if(!logicBoardTexturesLoaded)return;
    logicBoardTexturesLoaded = 0;
    UnloadTexture(baseTile);
    UnloadTexture(singleWires);
    UnloadTexture(doubleWires);
    UnloadTexture(oneInputGates);
    UnloadTexture(twoInputGates);
    UnloadTexture(converters);
    UnloadTexture(splitters);
    UnloadTexture(bridges);
    UnloadTexture(boardInputs);
    UnloadTexture(startSignal);
}

LogicBoard NewLogicBoard(int w, int h)
{
    LogicBoard newBoard = {
        0,
        0b0000000000000000,
        w,
        h,
        (LogicTile*)malloc(sizeof(LogicTile) * w * h)
    };
    //makes new board

    for(int i = 0; i < w * h; i++)
        newBoard.board[i] = (LogicTile){
            NOCIR,
            0,
            0b0000000000000000,
            0b0000000000000000
        };

    //initalizes data in board

    return newBoard;
}

void FreeLogicBoard(LogicBoard *lb)
{
    if(lb->board == NULL) return;
    free(lb->board);
    lb->board = NULL;
}

void DisplayLogicBoard(LogicBoard lb, Camera2D cam)
{
    if(lb.board == NULL) return;

    //finds what sections of the lb will be displayed on-screen
    Vector2 translatedPoint = GetScreenToWorld2D(
        (Vector2) {0, 0},
        cam
    );

    int minX = floorf(translatedPoint.x / 8.0f);
    int minY = floorf(translatedPoint.y / 8.0f);

    translatedPoint = GetScreenToWorld2D(
        (Vector2) {GetScreenWidth(), GetScreenHeight()},
        cam
    );

    int maxX = ceilf(translatedPoint.x / 8.0f);
    int maxY = ceilf(translatedPoint.y / 8.0f);

    if(maxX > lb.w)maxX = lb.w;
    if(maxY > lb.h)maxY = lb.h;
    if(minX < 0)minX = 0;
    if(minY < 0)minY = 0;

    //draws outline around logic board
    DrawRectangle(
        -2, -2, (lb.w << 3) + 4, (lb.h << 3) + 4,
        (Color) {0x26, 0x26, 0x28, 0xff}
    );

    DrawRectangle(-1, 0, 1, lb.h << 3, (Color) {0x1b, 0x1b, 0x1d, 0xff});
    DrawRectangle(0, -1, lb.w << 3, 1, (Color) {0x1b, 0x1b, 0x1d, 0xff});
    DrawRectangle(lb.w << 3, 0, 1, lb.h << 3, (Color) {0x3c, 0x3d, 0x3f, 0xff});
    DrawRectangle(0, lb.h << 3, lb.w << 3, 1, (Color) {0x3c, 0x3d, 0x3f, 0xff});

    for(int i = minY; i < maxY; i++)
    {
        for(int j = minX; j < maxX; j++)
            DrawTexture(baseTile, j << 3, i << 3, WHITE);//draws base tiles
    }

    DrawRectangle(0, 0, 1, lb.h << 3, (Color) {0x78, 0x79, 0x7d, 0xff});
    DrawRectangle(0, 0, lb.w << 3, 1, (Color) {0x78, 0x79, 0x7d, 0xff});

    DrawRectangle(
        (lb.w << 3) - 1, 0, 1, lb.h << 3, (Color) {0x78, 0x79, 0x7d, 0xff}
    );

    DrawRectangle(
        0, (lb.h << 3) - 1, lb.w << 3, 1, (Color) {0x78, 0x79, 0x7d, 0xff}
    );

    for(int i = minY; i < maxY; i++)
    {
        for(int j = minX; j < maxX; j++)
        {
            LogicTile curTile = lb.board[j + i * lb.w];//gets current tile

            int8_t shouldDraw = 0;
            Texture2D srcImg;
            float xSrc = 0.0f;
            float ySrc = 0.0f;

            switch(curTile.type)//draws circuits on top of the base tiles
            {
                case NOCIR:
                    break;


                case STRWIRE:
                case CURWIRE:
                    shouldDraw = 1;

                    srcImg = singleWires;

                    xSrc = (float)((GetPowerState(curTile)) << 3);
                    //gets the state of the tile type

                    ySrc = (float)((curTile.type - STRWIRE) << 3);
                    //gets ciruit type
                    break;

                case STRDWIRE:
                case CURDWIRE:
                    shouldDraw = 1;

                    srcImg = doubleWires;

                    xSrc = (float)((GetPowerState(curTile)) << 3);
                    //gets the state of the tile type

                    ySrc = (float)((curTile.type - STRDWIRE) << 3);
                    //gets ciruit type
                    break;

                case NOTGATE:
                case BUFGATE:
                    shouldDraw = 1;

                    srcImg = oneInputGates;

                    xSrc = (float)((GetPowerState(curTile)) << 3);
                    //gets the state of the tile type

                    ySrc = (float)((curTile.type - NOTGATE) << 3);
                    //gets ciruit type
                    break;

                case NORGATE:
                case ORGATE:
                case NANDGATE:
                case ANDGATE:
                case NXORGATE:
                case XORGATE:
                case OFFLATCH:
                case ONLATCH:
                    shouldDraw = 1;

                    srcImg = twoInputGates;

                    xSrc = (float)((GetPowerState(curTile)) << 3);
                    //gets the state of the tile type

                    ySrc = (float)((curTile.type - NORGATE) << 3);
                    //gets ciruit type
                    break;

                case LEFTCONVERTER:
                case RIGHTCONVERTER:
                case TCONVERTER:
                    shouldDraw = 1;

                    srcImg = converters;

                    xSrc = (float)((GetPowerState(curTile)) << 3);
                    //gets the state of the tile type

                    ySrc = (float)((curTile.type - LEFTCONVERTER) << 3);
                    //gets ciruit type
                    break;

                case LEFTSPLITTER:
                case RIGHTSPLITTER:
                case TSPLITTER:
                case FULLSPLITTER:
                    shouldDraw = 1;

                    srcImg = splitters;

                    xSrc = (float)((GetPowerState(curTile)) << 3);
                    //gets the state of the tile type

                    ySrc = (float)((curTile.type - LEFTSPLITTER) << 3);
                    //gets ciruit type
                    break;

                case DSINGLECORNER:
                case SINGLECROSS:
                    shouldDraw = 1;

                    srcImg = bridges;

                    xSrc = (float)((GetPowerState(curTile)) << 3);
                    //gets the state of the tile type

                    ySrc = (float)((curTile.type - DSINGLECORNER) << 3);
                    //gets ciruit type
                    break;

                case INPUTONE:
                case INPUTTWO:
                case INPUTTHREE:
                case INPUTFOUR:
                case INPUTFIVE:
                case INPUTSIX:
                case INPUTSEVEN:
                case INPUTEIGHT:
                case INPUTNINE:
                case INPUTTEN:
                case INPUTELEVEN:
                case INPUTTWELVE:
                case INPUTTHIRTEEN:
                case INPUTFOURTEEN:
                case INPUTFIFTEEN:
                case INPUTSIXTEEN: 
                    shouldDraw = 1;

                    srcImg = boardInputs;

                    xSrc = (float)((curTile.type - INPUTONE) << 3);
                    //gets input type

                    ySrc = (float)
                        ((0 != (lb.input & 
                        (1<<(curTile.type - INPUTONE)))) << 3);
                    //gets if the input is on
                    break;

                case STARTSIGNAL:
                    shouldDraw = 1;

                    srcImg = startSignal;

                    xSrc = (lb.tick == 0) << 3;
                    break;

                default:
                    break;
            }

            if(shouldDraw)
                DrawTexturePro(
                    srcImg,
                    (Rectangle){
                        xSrc,
                        //gets the state of the tile type

                        ySrc,
                        //gets ciruit type

                        8.0f,
                        8.0f
                    },
                    (Rectangle){
                        (float)(j << 3) + 4.0f,
                        (float)(i << 3) + 4.0f,
                        8.0f,8.0f
                    },
                    (Vector2){4.0f, 4.0f},
                    (float)curTile.rotation * 90.0f,
                    WHITE
                );
        }
    }
}

void TickLogicBoard(LogicBoard* lb)
{
    //updates prev power
    for(uint32_t y = 0; y < lb->h; y++)
    {
        for(uint32_t x = 0; x < lb->w; x++)
        {
            lb->board[y * lb->w + x].prevPower = lb->board[y * lb->w + x].power;
        }
    }

    lb->tick++;

    //updated power
    UpdatePowerLogicBoard(lb);
}

void UpdatePowerLogicBoard(LogicBoard* lb)
{
    //sets power to 0
    for(uint32_t y = 0; y < lb->h; y++)
    {
        for(uint32_t x = 0; x < lb->w; x++)
        {
            lb->board[y * lb->w + x].power = 0;
        }
    }

    //gets power of each cell
    for(uint32_t y = 0; y < lb->h; y++)
    {
        for(uint32_t x = 0; x < lb->w; x++)
        {
            UpdatePowerFromCell(lb, x, y, GetPowerOutput(*lb, x, y));
        }
    }
}

LogicBoard LoadLogicBoard(const char *path)
{
    FILE *filePtr = fopen(path, "rb");

    if(filePtr == NULL)return (LogicBoard) {0, 0, 0, 0, NULL};

    //used if I ever want to make another way of storing logic boards
    int32_t type;

    fread(&type, 4, 1, filePtr);

    LogicBoard retVal = (LogicBoard) {0, 0, 0, 0, NULL};

    int8_t *tempLtVals;
    LogicTile *Lts;

    switch(type)
    {
        case 0:

            //reads values from file
            fseek(filePtr, 4, SEEK_SET);
            fread(&retVal.tick, 4, 1, filePtr);

            fseek(filePtr, 8, SEEK_SET);
            fread(&retVal.input, 2, 1, filePtr);

            fseek(filePtr, 10, SEEK_SET);
            fread(&retVal.w, 4, 1, filePtr);

            fseek(filePtr, 14, SEEK_SET);
            fread(&retVal.h, 4, 1, filePtr);

            tempLtVals = (int8_t*) malloc(retVal.w * retVal.h << 2);

            fseek(filePtr, 18, SEEK_SET);

            fread(tempLtVals, 1, retVal.w * retVal.h << 2, filePtr);

            retVal.board = (LogicTile*) malloc(
                sizeof(LogicTile) * retVal.w * retVal.h
            );

            for(int i = 0; i < (retVal.w * retVal.h); i++)
            {
                retVal.board[i] = (LogicTile) {
                    tempLtVals[i << 2],
                    tempLtVals[(i << 2) + 1],
                    0,
                    tempLtVals[(i << 2) + 2] | (tempLtVals[(i << 2) + 3] << 8)
                };
            }

            free(tempLtVals);

            break;

        default:
            break;
    }

    return retVal;
}

void StoreLogicBoard(const char *path, LogicBoard lb)
{
    //makes the file if it does not already exist
    FILE *filePtr = fopen(path, "wb+");

    if(filePtr == NULL)return;

    printf("Saved board to %s\n", path);

    int8_t *tempLtVals;
    LogicTile *Lts = lb.board;

    //writes values from file
    int32_t type = 0;
    fwrite(&type, 4, 1, filePtr);

    fseek(filePtr, 4, SEEK_SET);
    fwrite(&lb.tick, 4, 1, filePtr);

    fseek(filePtr, 8, SEEK_SET);
    fwrite(&lb.input, 2, 1, filePtr);

    fseek(filePtr, 10, SEEK_SET);
    fwrite(&lb.w, 4, 1, filePtr);

    fseek(filePtr, 14, SEEK_SET);
    fwrite(&lb.h, 4, 1, filePtr);

    tempLtVals = (int8_t*) malloc(lb.w * lb.h << 2);

    for(int i = 0; i < (lb.w * lb.h); i++)
    {
        tempLtVals[i << 2] = lb.board[i].type;
        tempLtVals[(i << 2) + 1] = lb.board[i].rotation;
        tempLtVals[(i << 2) + 2] = lb.board[i].prevPower & 0xff;
        tempLtVals[(i << 2) + 3] = lb.board[i].prevPower >> 8;
    }

    fseek(filePtr, 18, SEEK_SET);
    fwrite(tempLtVals, 1, lb.w * lb.h << 2, filePtr);

    free(tempLtVals);

    fclose(filePtr);
}

/* exposed functions end */

/* NOTE: unexposed function definitions */

int GetPowerState(LogicTile lt)
{
    //how left side is powered adjusted for rotation
    uint8_t left = (lt.power >> (((3 - lt.rotation) & 3) << 2)) & 7;
    //how left side is powered adjusted for rotation
    uint8_t top = (lt.power >> (((2 - lt.rotation) & 3) << 2)) & 7;
    //how left side is powered adjusted for rotation
    uint8_t right = (lt.power >> (((1 - lt.rotation) & 3) << 2)) & 7;
    //how left side is powered adjusted for rotation
    uint8_t bottom = (lt.power >> (((0 - lt.rotation) & 3) << 2)) & 7;

    //how left side is powered adjusted for rotation
    uint8_t prevLeft = (lt.prevPower >> (((3 - lt.rotation) & 3) << 2)) & 7;
    //how left side is powered adjusted for rotation
    uint8_t prevTop = (lt.prevPower >> (((2 - lt.rotation) & 3) << 2)) & 7;
    //how left side is powered adjusted for rotation
    uint8_t prevRight = (lt.prevPower >> (((1 - lt.rotation) & 3) << 2)) & 7;
    //how left side is powered adjusted for rotation
    uint8_t prevBottom = (lt.prevPower >> (((0 - lt.rotation) & 3) << 2)) & 7;

    switch(lt.type)
    {
        case STRWIRE:
            return (top | bottom) & 1;

        case CURWIRE:
            return (top | right) & 1;

        case STRDWIRE:
            return ((RotateSidePower(top) | bottom) & 6) >> 1;

        case CURDWIRE:
            return ((RotateSidePower(top) | right) & 6) >> 1;

        case BUFGATE:
        case NOTGATE:
            return (bottom & 1) | ((prevBottom & 1) << 1);

        case NORGATE:
        case ORGATE:
        case NANDGATE:
        case ANDGATE:
        case NXORGATE:
        case XORGATE:
        case OFFLATCH:
        case ONLATCH:
            return ((bottom & 2) >> 1) | (prevBottom & 2) |
                (bottom & 4) | ((prevBottom & 4) << 1);

        case LEFTCONVERTER:
            return (left & 1) | ((bottom & 1) << 1) |
                ((RotateSidePower(top) & 6) >> 1);

        case RIGHTCONVERTER:
            return (bottom & 1) | ((right & 1) << 1) |
                ((RotateSidePower(top) & 6) >> 1);

        case TCONVERTER:
            return (left & 1) | ((right & 1) << 1) |
                ((RotateSidePower(top) & 6) >> 1);

        case LEFTSPLITTER:
        case RIGHTSPLITTER:
        case TSPLITTER:
        case FULLSPLITTER:
            return bottom & 1;

        case SINGLECROSS:
            return ((bottom | top) & 1) | (((left | right) & 1) << 1);

        case DSINGLECORNER:
            return ((bottom | left) & 1) | (((top | right) & 1) << 1);

        default:
            break;
    }

    return 0;
}

int8_t RotateSidePower(int8_t sidePower)
{
    return (sidePower & 1) | ((sidePower & 2) << 1) | ((sidePower & 4) >> 1);
}


void UpdatePowerFromCell(
    LogicBoard *lb,
    uint32_t x,
    uint32_t y,
    int16_t powerOutput
)
{
    //the pervious power of cells around the current one
    int16_t prevCellPower;

    //uses the power values form each side to update other power vals
    if(x > 0)
    {
        prevCellPower = lb->board[y * lb->w + (x - 1)].power;

        lb->board[y * lb->w + (x - 1)].power |= (powerOutput & 0xf000) >> 8;

        if(prevCellPower != lb->board[y * lb->w + (x - 1)].power)
            UpdatePowerFromCell(
                lb, x - 1, y, GetPowerOutput(*lb, x - 1, y)
            );
    }

    if(y > 0)
    {
        prevCellPower = lb->board[(y - 1) * lb->w + x].power;

        lb->board[(y - 1) * lb->w + x].power |= (powerOutput & 0x0f00) >> 8;

       if(prevCellPower != lb->board[(y - 1) * lb->w + x].power)
            UpdatePowerFromCell(
                lb, x, y - 1, GetPowerOutput(*lb, x, y - 1)
            );
    }

    if((x + 1) < lb->w)
    {
        prevCellPower = lb->board[y * lb->w + (x + 1)].power;

        lb->board[y * lb->w + (x + 1)].power |= (powerOutput & 0x00f0) << 8;

       if(prevCellPower != lb->board[y * lb->w + (x + 1)].power)
            UpdatePowerFromCell(
                lb, x + 1, y, GetPowerOutput(*lb, x + 1, y)
            );
    }

    if((y + 1) < lb->h)
    {
        prevCellPower = lb->board[(y + 1) * lb->w + x].power;

        lb->board[(y + 1) * lb->w + x].power |= (powerOutput & 0x000f) << 8;

       if(prevCellPower != lb->board[(y + 1) * lb->w + x].power)
            UpdatePowerFromCell(
                lb, x, y + 1, GetPowerOutput(*lb, x, y + 1)
            );
    }
}

int16_t GetPowerOutput(LogicBoard lb, uint32_t x, uint32_t y)
{
    //the current tile specified by the given position
    LogicTile lt = lb.board[y * lb.w + x];

    //rotates power
    int16_t rotatedPower = rotatePower(lt.power, lt.rotation);

    //input used for logic gates
    int8_t centerInput = rotatePower(lt.prevPower, lt.rotation) & 7;
    int8_t leftInput = centerInput >> 1;
    int8_t rightInput = leftInput >> 1;
    centerInput &= 1;
    leftInput &= 1;
    rightInput &= 1;

    switch(lt.type)
    {
        case INPUTONE:
        case INPUTTWO:
        case INPUTTHREE:
        case INPUTFOUR:
        case INPUTFIVE:
        case INPUTSIX:
        case INPUTSEVEN:
        case INPUTEIGHT:
        case INPUTNINE:
        case INPUTTEN:
        case INPUTELEVEN:
        case INPUTTWELVE:
        case INPUTTHIRTEEN:
        case INPUTFOURTEEN:
        case INPUTFIFTEEN:
        case INPUTSIXTEEN:
            return 0x1111 * ((lb.input >> (lt.type - INPUTONE)) & 1);

        case STARTSIGNAL:
            return 0x1111 * (lb.tick == 0);

        case STRWIRE:
            return rotatePower(
                0x0101 * ((rotatedPower & 0x0101) != 0),
                4 - lt.rotation
            );

        case CURWIRE:
            return rotatePower(
                0x0110 * ((rotatedPower & 0x0110) != 0),
                4 - lt.rotation
            );

        case STRDWIRE:
            return rotatePower(
                (0x0204 * ((rotatedPower & 0x0402) != 0)) |
                (0x0402 * ((rotatedPower & 0x0204) != 0)),
                4 - lt.rotation
            );

        case CURDWIRE:
            return rotatePower(
                (0x0420 * ((rotatedPower & 0x0240) != 0)) |
                (0x0240 * ((rotatedPower & 0x0420) != 0)),
                4 - lt.rotation
            );

        case LEFTSPLITTER:
            return rotatePower(
                0x1100 * ((rotatedPower & 0x0001) != 0),
                4 - lt.rotation
            );

        case RIGHTSPLITTER:
            return rotatePower(
                0x0110 * ((rotatedPower & 0x0001) != 0),
                4 - lt.rotation
            );

        case TSPLITTER:
            return rotatePower(
                0x1010 * ((rotatedPower & 0x0001) != 0),
                4 - lt.rotation
            );

        case FULLSPLITTER:
            return rotatePower(
                0x1110 * ((rotatedPower & 0x0001) != 0),
                4 - lt.rotation
            );

        case SINGLECROSS:
            return rotatePower(
                (0x0101 * ((rotatedPower & 0x0101) != 0)) |
                (0x1010 * ((rotatedPower & 0x1010) != 0)),
                4 - lt.rotation
            );

        case DSINGLECORNER:
            return rotatePower(
                (0x0110 * ((rotatedPower & 0x0110) != 0)) |
                (0x1001 * ((rotatedPower & 0x1001) != 0)),
                4 - lt.rotation
            );

        case LEFTCONVERTER:
            return rotatePower(
                (0x0401 * ((rotatedPower & 0x0201) != 0)) |
                (0x1200 * ((rotatedPower & 0x1400) != 0)),
                4 - lt.rotation
            );

        case RIGHTCONVERTER:
            return rotatePower(
                (0x0410 * ((rotatedPower & 0x0210) != 0)) |
                (0x0201 * ((rotatedPower & 0x0401) != 0)),
                4 - lt.rotation
            );

        case TCONVERTER:
            return rotatePower(
                (0x0410 * ((rotatedPower & 0x0210) != 0)) |
                (0x1200 * ((rotatedPower & 0x1400) != 0)),
                4 - lt.rotation
            );

        case NOTGATE:
            return rotatePower(0x0100 * !centerInput, 4 - lt.rotation);

        case BUFGATE:
            return rotatePower(0x0100 * centerInput, 4 - lt.rotation);

        case NANDGATE:
            return rotatePower(
                0x0100 * !(leftInput & rightInput),
                4 - lt.rotation);

        case ANDGATE:
            return rotatePower(
                0x0100 * (leftInput & rightInput),
                4 - lt.rotation);

        case NORGATE:
            return rotatePower(
                0x0100 * !(leftInput | rightInput),
                4 - lt.rotation);

        case ORGATE:
            return rotatePower(
                0x0100 * (leftInput | rightInput),
                4 - lt.rotation);

        case NXORGATE:
            return rotatePower(
                0x0100 * !(leftInput ^ rightInput),
                4 - lt.rotation);

        case XORGATE:
            return rotatePower(
                0x0100 * (leftInput ^ rightInput),
                4 - lt.rotation);

        case OFFLATCH:
        case ONLATCH:

            //updates state of logic tile
            if(leftInput)
            {
                lb.board[y * lb.w + x].type = OFFLATCH + rightInput;
                lt = lb.board[y * lb.w + x];
            }

            return rotatePower(
                0x0100 * !((ONLATCH - lt.type) & 1),
                4 - lt.rotation);

        default:
            return 0;
    }
}

int16_t rotatePower(int16_t power, int8_t rotation)
{
    uint32_t temp = power << (rotation << 2);
    temp |= (temp & 0xffff0000) >> 16;
    power = temp & 0xffff;
    return power;
}

/* unexposed function definitions end */

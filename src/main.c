#include "../include/main.h"
#include <raylib.h>
#include <stdio.h>

//a struct created to manage the state of the menu
typedef struct Menu
{
    //position of the menu
    Vector2 position;

    //source image for the menu
    Texture2D srcImg;

    //number of elements
    int elemNum;

    //where the elements can be clicked
    Rectangle *elemHitboxes;

    //positions of elements relative to position
    Rectangle *elemPos;

    //if the mouse went up over an element
    char *wasClicked;

    //if the mouse went down over an element
    char *isClicked;

    //if an element has the mouse over it
    char *mouseOver;

    //stores data about each element that updates when the element is clicked
    int *clickTracker;

    //if the mouse went down somewhere over the menu
    char menuClicked;

    //element sources
    Rectangle *elemSrcs;

    //scale of the elements in the menu
    float scale;

    //menu bounds
    Rectangle menuBounds;

    //the type of tile that is selected
    int tileType;
} Menu;

//represents a clipboard
typedef struct Clipboard {
    //x pos of selected section
    int x;

    //y pos of selected section
    int y;

    //width of selected section
    int w;

    //height of selected section
    int h;

    LogicBoard board;
} Clipboard;

int main(int argc, char **argv)
{
    //sets path
    SetPathToDir(*argv);

    //counts how many frames have been rendered
    int frameCounter = 0;

    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "project");
    LoadLogicBoardTextures();

    char *tempStr = AddPath("/sprites/logicBoardMenu.png");
    Texture2D menuTexture = LoadTexture(tempStr);
    AddPathReuse("/sprites/boardBackground.png", &tempStr);
    Texture2D backgroundTexture = LoadTexture(tempStr);
    free(tempStr);

    LogicBoard mainBoard = LoadLogicBoard("./boards/mainboard.bin");

    if(mainBoard.w == 0) mainBoard = NewLogicBoard(25, 25);

    //used later
    Vector2 selectedPos = {-1.0f, -1.0f};
    Vector2 mouseOnMenuPos = {-1.0f, -1.0f};

    //the main camera used for displaying the game world
    Camera2D worldCamera = {
        .target = (Vector2){
            (float)(mainBoard.w << 2),
            (float)(mainBoard.h << 2)
        },
        .offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f},
        .rotation = 0.0f,
        .zoom = 2.0f
    };

    //dragTracker for worldCamera
    DragTracker worldCamDrag = {
        &worldCamera.target,
        (Rectangle){
            0.0f,
            0.0f,
            (float)(mainBoard.w << 3),
            (float)(mainBoard.h << 3)
        },
        &worldCamera,
        -1.0f,
        (Vector2){0.0f, 0.0f},
        (Vector2){0.0f, 0.0f}
    };
    char worldCamWasDragged = 0;

    //clipboard handling
    Clipboard mainCb = {0, 0, 0, 0, NewLogicBoard(0, 0)};

    DragTracker clipboardTracker = {
        NULL,
        (Rectangle){0, 0, 0, 0},
        &worldCamera,
        1.0f,
        (Vector2){0.0f, 0.0f},
        (Vector2){0.0f, 0.0f}
    };

    //menu handling

    //fitboxes of each element
    Rectangle elemHitBoxes[] = {
        (Rectangle) {2.0f, 2.0f, 38.0f, 38.0f},
        (Rectangle) {43.0f, 2.0f, 6.0f, 8.0f},
        (Rectangle) {43.0f, 12.0f, 6.0f, 8.0f},
        (Rectangle) {43.0f, 24.0f, 6.0f, 5.0f},
        (Rectangle) {43.0f, 33.0f, 6.0f, 6.0f},
        (Rectangle) {0.0f, 0.0f, 0.0f, 0.0f},
        (Rectangle) {0.0f, 0.0f, 0.0f, 0.0f},
        (Rectangle) {2.0f, 44.0f, 6.0f, 8.0f},
        (Rectangle) {10.0f, 44.0f, 6.0f, 8.0f},
        (Rectangle) {18.0f, 44.0f, 6.0f, 8.0f},
        (Rectangle) {26.0f, 44.0f, 6.0f, 8.0f},
        (Rectangle) {34.0f, 44.0f, 6.0f, 8.0f},
        (Rectangle) {42.0f, 44.0f, 6.0f, 8.0f},
        (Rectangle) {0.0f, 0.0f, 0.0f, 0.0f},
    };

    //positions of each element
    Rectangle elemPos[] = {
        (Rectangle) {0.0f, 0.0f, 42.0f, 42.0f},
        (Rectangle) {46.0f, 6.0f, 10.0f, 12.0f},
        (Rectangle) {42.0f, 12.0f, 9.0f, 11.0f},
        (Rectangle) {42.0f, 23.0f, 9.0f, 10.0f},
        (Rectangle) {42.0f, 33.0f, 9.0f, 9.0f},
        (Rectangle) {50.0f, 0.0f, 52.0f, 42.0f},
        (Rectangle) {101.0f, 0.0f, 52.0f, 42.0f},
        (Rectangle) {0.0f, 42.0f, 8.0f, 12.0f},
        (Rectangle) {8.0f, 42.0f, 8.0f, 12.0f},
        (Rectangle) {16.0f, 42.0f, 8.0f, 12.0f},
        (Rectangle) {24.0f, 42.0f, 8.0f, 12.0f},
        (Rectangle) {32.0f, 42.0f, 8.0f, 12.0f},
        (Rectangle) {40.0f, 42.0f, 11.0f, 12.0f},
        (Rectangle) {147.0f, 42.0f, 6.0f, 12.0f},
    };

    //soureces of each element
    Rectangle elemSrcs[] = {
        (Rectangle) {0.0f, 0.0f, 42.0f, 42.0f},
        (Rectangle) {41.0f, 0.0f, 10.0f, 12.0f},
        (Rectangle) {42.0f, 12.0f, 9.0f, 11.0f},
        (Rectangle) {42.0f, 23.0f, 9.0f, 10.0f},
        (Rectangle) {42.0f, 33.0f, 9.0f, 9.0f},
        (Rectangle) {50.0f, 0.0f, 52.0f, 42.0f},
        (Rectangle) {51.0f, 42.0f, 52.0f, 42.0f},
        (Rectangle) {0.0f, 84.0f, 8.0f, 12.0f},
        (Rectangle) {8.0f, 84.0f, 8.0f, 12.0f},
        (Rectangle) {16.0f, 84.0f, 8.0f, 12.0f},
        (Rectangle) {24.0f, 84.0f, 8.0f, 12.0f},
        (Rectangle) {32.0f, 84.0f, 8.0f, 12.0f},
        (Rectangle) {40.0f, 84.0f, 11.0f, 12.0f},
        (Rectangle) {98.0f, 96.0f, 6.0f, 12.0f},
    };

    //matches positions on the menu with tiletypes
    int tileTypeArray[] =
    {
        INPUTONE, INPUTTWO, INPUTTHREE, INPUTFOUR,
        ANDGATE, NANDGATE, LEFTCONVERTER, STRDWIRE,
        CURDWIRE, STARTSIGNAL, INPUTFIVE, INPUTSIX,
        INPUTSEVEN, INPUTEIGHT, ORGATE, NORGATE,
        RIGHTCONVERTER, STRWIRE, CURWIRE, OFFLATCH,
        INPUTNINE, INPUTTEN, INPUTELEVEN, INPUTTWELVE,
        XORGATE, NXORGATE, TCONVERTER, SINGLECROSS,
        DSINGLECORNER, ONLATCH, INPUTTHIRTEEN, INPUTFOURTEEN,
        INPUTFIFTEEN, INPUTSIXTEEN, BUFGATE, NOTGATE,
        TSPLITTER, LEFTSPLITTER, RIGHTSPLITTER, FULLSPLITTER
    };

    //the menu struct itself
    Menu boardMenu = {
        .position = (Vector2){0.0f, 0.0f},
        .srcImg = menuTexture,
        .scale = 2.0f,
        .elemNum = 14,
        .elemHitboxes = elemHitBoxes,
        .elemPos = elemPos,
        .elemSrcs = elemSrcs,
        .menuBounds = (Rectangle){0.0f, 0.0f, 152.0f, 54.0f},
        .menuClicked = 0,
        .wasClicked = (char[]) {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .isClicked = (char[]) {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .mouseOver = (char[]) {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .clickTracker = (int[]) {0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 3, 0, -1},
        .tileType = 0
    };

    //sets position of menu
    boardMenu.scale = ((float) GetScreenWidth()) / 400.0f;
    if(boardMenu.scale < 1.0f)boardMenu.scale = 1.0f;
    if(boardMenu.scale > 4.0f)boardMenu.scale = 4.0f;
    boardMenu.scale *= 1.5;
    boardMenu.position.x = (
            ((float) GetScreenWidth()) -
            (boardMenu.menuBounds.width * boardMenu.scale)
        ) * 0.5f;
    boardMenu.position.y = ((float) GetScreenHeight()) -
        boardMenu.menuBounds.height * boardMenu.scale;


    //drag tracker for menu
    DragTracker menuDrag = {
        &boardMenu.position,
        (Rectangle){
            0.0f,
            0.0f,
            screenWidth,
            screenHeight
        },
        NULL,
        1.0f,
        (Vector2){0.0f, 0.0f},
        (Vector2){0.0f, 0.0f}
    };

    //does what it says
    SetTargetFPS(30);

    UpdatePowerLogicBoard(&mainBoard);

    //game loop for creating logical circuits
    /* NOTE: GAME LOOP*/
    while (!WindowShouldClose())
    {
        /* NOTE: LOGIC SECTION*/

        //gets dims of screen
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        //repositions camera
        worldCamera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};

        //updates world camera zoom based on mouse zoom
        zoomCamera(&worldCamera, 1.0f, 64.0f, 0.05f);

        //updates menu scale based on screen size
        boardMenu.scale = ((float) screenWidth) / 400.0f;
        if(boardMenu.scale < 1.0f)boardMenu.scale = 1.0f;
        if(boardMenu.scale > 4.0f)boardMenu.scale = 4.0f;
        boardMenu.scale *= 1.5;

        //handels when the mouse is pressed down
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            worldCamWasDragged = 0;
            //resets worldCamWasDragged

            //if menu was clicked
            if(CheckCollisionPointRec(
                GetMousePosition(),
                (Rectangle) {
                    boardMenu.position.x, boardMenu.position.y,
                    boardMenu.menuBounds.width * boardMenu.scale,
                    boardMenu.menuBounds.height * boardMenu.scale
                }
            ))
            {
                //sets menuClicked to true
                boardMenu.menuClicked = 1;

                //resets the input clicktracker to -1
                boardMenu.clickTracker[13] = -1;

                //checks if each member of the menu ui is clicked
                for(int i = 0; i < boardMenu.elemNum; i++)
                    boardMenu.isClicked[i] = CheckCollisionPointRec(
                        GetMousePosition(),
                        (Rectangle) {
                            boardMenu.position.x +
                            boardMenu.elemHitboxes[i].x * boardMenu.scale,
                            boardMenu.position.y +
                            boardMenu.elemHitboxes[i].y * boardMenu.scale,
                            boardMenu.elemHitboxes[i].width * boardMenu.scale,
                            boardMenu.elemHitboxes[i].height * boardMenu.scale
                        }
                    );

                //sets the first ui elements (screen dragger) click tracker to
                //if it was clicked
                *boardMenu.clickTracker = *boardMenu.isClicked;
            } else
            {
                //if the menu was not clicked, updates this accordingly
                boardMenu.menuClicked = 0;
            }
        }

        //when the mouse goes up after being clicked
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {

            //updates wasClicked if an element was clicked
            for(int i = 0; i < boardMenu.elemNum; i++)
                boardMenu.wasClicked[i] = CheckCollisionPointRec(
                    GetMousePosition(),
                    (Rectangle) {
                        boardMenu.position.x +
                        boardMenu.elemHitboxes[i].x * boardMenu.scale,
                        boardMenu.position.y +
                        boardMenu.elemHitboxes[i].y * boardMenu.scale,
                        boardMenu.elemHitboxes[i].width * boardMenu.scale,
                        boardMenu.elemHitboxes[i].height * boardMenu.scale
                    }
                );

            //sets clickTragker for menu dragger to 0 when the mouse goes up
            *boardMenu.clickTracker = 0;

            //if the menu was not clicked and the world cam was not dragged
            if(
                !(boardMenu.menuClicked || worldCamWasDragged)
            )
            {
                //gets the position of the mouse in world space
                Vector2 mouseInWorldPos = GetScreenToWorld2D(
                    GetMousePosition(),
                    worldCamera
                );

                int x = (int)(mouseInWorldPos.x / 8.0f);
                int y = (int)(mouseInWorldPos.y / 8.0f);

                //if the logic board itself was clicked
                if(
                    (mouseInWorldPos.y > 0 && mouseInWorldPos.y > 0) &&
                    (x < mainBoard.w) && (y < mainBoard.h) &&
                    !boardMenu.clickTracker[2]
                )
                {
                    //gets the index of the clicked position
                    int index = mainBoard.w * ((int)(mouseInWorldPos.y / 8.0f))
                        + ((int)(mouseInWorldPos.x / 8.0f));

                    //if the rotation is the same as the clicked peice
                    if(
                         mainBoard.board[index].rotation ==
                        (boardMenu.clickTracker[1] ^ ((
                        boardMenu.clickTracker[1] & 1) << 1))
                    )
                    {
                        if(mainBoard.board[index].type == boardMenu.tileType)
                            mainBoard.board[index].type = NOCIR;
                        else
                            mainBoard.board[index] = (LogicTile) {
                                boardMenu.tileType,
                                boardMenu.clickTracker[1] ^ ((
                                boardMenu.clickTracker[1] & 1) << 1),
                                0, 0
                            };
                    }
                    else
                        //sets the clicked piece to the selected type
                        mainBoard.board[index] = (LogicTile) {
                            boardMenu.tileType,
                            boardMenu.clickTracker[1] ^ ((
                            boardMenu.clickTracker[1] & 1) << 1),
                            0, 0
                        };

                    //updates the power of the logic board
                    UpdatePowerLogicBoard(&mainBoard);
                    boardMenu.clickTracker[7] = 0;
                }
            }
        }

        //if the menu is clicked
        if(boardMenu.menuClicked)
        {

            //if the menu dragger is clicked, updates menu drag
            if(boardMenu.isClicked[0])
            {
                updateDragTracker(&menuDrag);
            }

            //when the button is released over the menu, updates click trackers
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                //loops through main click trackers for buttons
                for(int i = 1; i < 6; i++)
                {
                    if(i == 5)i = 10;

                    if(boardMenu.wasClicked[i] && boardMenu.isClicked[i])
                    {
                        boardMenu.clickTracker[i]++;

                        //all trackers can be 1 or 0, except 1 which can be 0-3
                        boardMenu.clickTracker[i] &= ((i == 1)? 3 : 1);

                        //resets width of clipboard when selection mode ends
                        if(i == 2)mainCb.w = 0;
                    }
                }

                if(boardMenu.clickTracker[10] == 1)
                {
                    //resets frame counter to cause a tick this frame
                    frameCounter = 0;
                }

                //handles other buttons

                //saves
                if(
                    (boardMenu.wasClicked[7] && boardMenu.isClicked[7]) ||
                    (boardMenu.wasClicked[8] && boardMenu.isClicked[8])
                )
                {
                    boardMenu.clickTracker[7] = 1;
                    boardMenu.clickTracker[10] = 0;
                    StoreLogicBoard("./boards/mainboard.bin", mainBoard);
                }

                //quits
                if(
                    (boardMenu.wasClicked[8] && boardMenu.isClicked[8]) ||
                    (boardMenu.wasClicked[9] && boardMenu.isClicked[9])
                )
                {
                    //frees allocated mem
                    UnloadLogicBoardTextures();
                    UnloadTexture(menuTexture);
                    UnloadTexture(backgroundTexture);
                    FreeLogicBoard(&mainBoard);
                    FreeLogicBoard(&mainCb.board);
                    return 0;
                }

                if(
                    boardMenu.wasClicked[11] &&
                    boardMenu.isClicked[11] &&
                    (mainBoard.tick != 0)
                )
                {
                    //resets tick to 0
                    mainBoard.tick = 0;

                    boardMenu.clickTracker[11] = 1;

                    //resets prev power
                    for(int i = 0; i < mainBoard.h; i++)
                    {
                        for(int j = 0; j < mainBoard.w; j++)
                        {
                            mainBoard.board[i * mainBoard.w + j].prevPower = 0;
                        }
                    }

                    UpdatePowerLogicBoard(&mainBoard);

                    //stops the board from ticking after reset
                    boardMenu.clickTracker[10] = 0;
                }

                //if the tick once buton is clicked, tickes once
                if(boardMenu.wasClicked[12] && boardMenu.isClicked[12])
                {
                    TickLogicBoard(&mainBoard);
                    boardMenu.clickTracker[11] = 0;
                    boardMenu.clickTracker[7] = 0;
                }

                //updates menu clicked
                boardMenu.menuClicked = 0;
            }
        } else //if the menu was not clicked
        {
            //if in selection mode
            if(boardMenu.clickTracker[2])
            {
                //updates clipboard selection
                updateDragTracker(&clipboardTracker);

                //if the mouse is down
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    //gets mouse change
                    Vector2 t = clipboardTracker.mouseChange;

                    //converts it to world space
                    t = GetScreenToWorld2D(t, worldCamera);

                    //gets mouseChange offset
                    Vector2 mso = (Vector2) {
                        t.x - (floorf(t.x / 8.0f) * 8.0f),
                        t.y - (floorf(t.y / 8.0f) * 8.0f)
                    };

                    //rounds position and updates clipboard
                    mainCb.x = ((int) floorf(t.x)) >> 3;
                    mainCb.y = ((int) floorf(t.y)) >> 3;
                    t = (Vector2) {
                        (float) (mainCb.x << 3),
                        (float) (mainCb.y << 3)
                    };

                    //updates width and height of copied area
                    mainCb.w =
                        (int) (
                            (GetWorldDragX(clipboardTracker) + mso.x) / 8.0f
                        );
                    if(((GetWorldDragX(clipboardTracker) + mso.x) / 8.0f) < 0)
                        mainCb.w--;
                    mainCb.h =
                        (int) (
                            (GetWorldDragY(clipboardTracker) + mso.y) / 8.0f
                        );
                    if(((GetWorldDragY(clipboardTracker) + mso.y) / 8.0f) < 0)
                        mainCb.h--;

                    //handles negitive width and height
                    if(mainCb.w < 0)
                    {
                        mainCb.x += mainCb.w;
                        mainCb.w *= -1;
                    }

                    if(mainCb.h < 0)
                    {
                        mainCb.y += mainCb.h;
                        mainCb.h *= -1;
                    }

                    mainCb.w++;
                    mainCb.h++;

                    //prevents out of bounds selection
                    if(
                        ((mainCb.x + mainCb.w) > 0) &&
                        ((mainCb.y + mainCb.h) > 0)
                    )
                    {
                        if(mainCb.y < 0)
                        {
                            mainCb.h += mainCb.y;
                            mainCb.y = 0;
                        }

                        if(mainCb.x < 0)
                        {
                            mainCb.w += mainCb.x;
                            mainCb.x = 0;
                        }

                        if((mainCb.x + mainCb.w) > mainBoard.w)
                            mainCb.w = mainBoard.w - mainCb.x;

                        if((mainCb.y + mainCb.h) > mainBoard.h)
                            mainCb.h = mainBoard.h - mainCb.y;
                    } else
                    {
                        mainCb.w = 0;
                        mainCb.h = 0;
                    }
                }
            }
            else //if not in selection mode
            {
                //updates drag tracker and wasDragged
                updateDragTracker(&worldCamDrag);
                worldCamWasDragged |= !((fabsf(GetDragX(worldCamDrag)) < 4.0f)
                    && (fabsf(GetDragY(worldCamDrag)) < 4.0f));
            }
        }

        //changes menu width based on if the menu is minimized
        boardMenu.menuBounds.width = 
            (
                (boardMenu.clickTracker[4] || boardMenu.clickTracker[2])
                ? 51.0f : 153.0f
            );

        //changes menu height based on if the menu is minimized
        boardMenu.menuBounds.height =
                (boardMenu.clickTracker[3]) ? 54.0f : 42.0f;

        //changes bounds on drag tracker for menu to reflect updated width
        menuDrag.bounds = (Rectangle) {
            0.0f,
            0.0f,
            screenWidth - (boardMenu.menuBounds.width * boardMenu.scale),
            screenHeight - (boardMenu.menuBounds.height * boardMenu.scale)
        };

        //updates drag of menu
        boundDragTracker(&menuDrag);

        //keyboard shortcuts for selection mode
        if(boardMenu.clickTracker[2])
        {
            //x cuts which copies then deletes

            //c for copy
            if(IsKeyPressed(KEY_C) || IsKeyPressed(KEY_X))
            {
                if(mainCb.board.board != NULL)FreeLogicBoard(&mainCb.board);

                mainCb.board = NewLogicBoard(mainCb.w, mainCb.h);

                for(int x = 0; x < mainCb.w; x++)
                {
                    for(int y = 0; y < mainCb.h; y++)
                    {
                        mainCb.board.board[x + y * mainCb.w] = mainBoard.board[
                            x + mainCb.x + (y + mainCb.y) * mainBoard.w
                        ];
                    }
                }
            }

            //v for paste
            if(IsKeyPressed(KEY_V))
            {
                for
                (
                    int x = 0;
                    (x < mainCb.board.w) && (x + mainCb.x < mainBoard.w);
                    x++
                )
                {
                    for
                    (
                        int y = 0;
                        (y < mainCb.board.h) && (y + mainCb.y < mainBoard.h);
                        y++
                    )
                    {
                        mainBoard.board[
                            x + mainCb.x + (y + mainCb.y) * mainBoard.w
                        ] = mainCb.board.board[x + y * mainCb.board.w];
                    }
                }

                //updates the power of the logic board
                UpdatePowerLogicBoard(&mainBoard);
                boardMenu.clickTracker[7] = 0;
            }

            //delete to remove the selectd area
            if(IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_X))
            {
                for(int x = 0; x < mainCb.w; x++)
                {
                    for(int y = 0; y < mainCb.h; y++)
                    {
                        mainBoard.board[
                            x + mainCb.x + (y + mainCb.y) * mainBoard.w
                        ] = (LogicTile) {0, 0, 0, 0};
                    }
                }

                //updates the power of the logic board
                UpdatePowerLogicBoard(&mainBoard);
                boardMenu.clickTracker[7] = 0;
            }
        }

        //if the menu is not minimized
        if(!(boardMenu.clickTracker[4] || boardMenu.clickTracker[2]))
        {
            //resets click tracker that tracks what circuit type the mouse is over
            boardMenu.clickTracker[5] = -1;

            //resets mouse on menu pos
            mouseOnMenuPos.x = -1.0f;

            for(int i = 0; i < 10; i++)
            {
                for(int j = 0; j < 4; j++)
                {

                    //if the mouse is over the given circuit type
                    if(CheckCollisionPointRec(
                        GetMousePosition(),
                        (Rectangle) {
                            boardMenu.position.x + 
                            (float)(52 + 10 * i) * boardMenu.scale,
                            boardMenu.position.y + 
                            (float)(2 + 10 * j) * boardMenu.scale,
                            8.0f * boardMenu.scale,
                            8.0f * boardMenu.scale
                        }
                    ))
                    {

                        //updates the last circuit type that the mouse was over
                        boardMenu.clickTracker[5] = j * 10 + i;

                        //when the mouse is clicked, updates this click tracker
                        //to the current circuit type the mouse is over
                        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                            boardMenu.clickTracker[6] = j * 10 + i;

                        //if the mouse was released over the menu
                        if(
                            IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                            (boardMenu.clickTracker[6] != -1)
                        )
                        {

                            //unselects type if type already selected
                            if(
                                tileTypeArray[boardMenu.clickTracker[6]] ==
                                boardMenu.tileType
                            )
                            {
                                boardMenu.tileType = 0;
                                selectedPos = (Vector2){-1.0f, -1.0f};
                            } else
                            //otherwise selects the type
                            {
                                boardMenu.tileType =
                                    tileTypeArray[boardMenu.clickTracker[6]];

                                selectedPos = (Vector2){
                                    (float)(52 + 10 * i),
                                    (float)(2 + 10 * j)
                                };
                            }
                        }

                        //if the mouse is nt on the same type as the type that
                        //was clicked, then the type is unselected
                        if(
                            (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
                            (boardMenu.clickTracker[6] !=
                            boardMenu.clickTracker[5])
                        ))boardMenu.clickTracker[6] = -1;

                        //sets marker of where the mouse is over
                        mouseOnMenuPos.x = (float)(51 + 10 * i);
                        mouseOnMenuPos.y = (float)(1 + 10 * j);
                    }
                }
            }

            //resets selected type if the type that the mouse is over is not the
            //same as the type that was clicked
            if(boardMenu.clickTracker[5] == -1)boardMenu.clickTracker[6] = -1;
        }

        //if the menu is fully displayed
        if(
            !boardMenu.clickTracker[2] &&
            !boardMenu.clickTracker[4] &&
            boardMenu.clickTracker[3]
        )
        {

            //loops through the inputs
            for(int i = 0; i < 16; i++)
            {

                //if one of the inputs is clicked
                if(CheckCollisionPointRec(
                    GetMousePosition(),
                    (Rectangle) {
                        boardMenu.position.x + 
                        (52.0f + ((float)(i * 6))) * boardMenu.scale,
                        boardMenu.position.y + 
                        48.0f * boardMenu.scale,
                        4.0f * boardMenu.scale,
                        4.0f * boardMenu.scale
                    }
                ))
                {

                    //sets the tracker when the relevant input is clicked
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {
                        boardMenu.clickTracker[13] = i;
                    }

                    //changes input when button is released
                    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        if(boardMenu.clickTracker[13] == i)
                            mainBoard.input ^= 1 << i;

                        //updates the power of the logic board
                        UpdatePowerLogicBoard(&mainBoard);
                        boardMenu.clickTracker[7] = 0;
                    }
                }
            }
        }

        if(boardMenu.clickTracker[10])
        {
            if(frameCounter == 0)
            {
                TickLogicBoard(&mainBoard);
                boardMenu.clickTracker[7] = 0;

                //allows the board to be reset after a tick
                boardMenu.clickTracker[11] = 0;
            }
            frameCounter++;
            frameCounter &= 3;
        }

        /* LOGIC SECTION END*/
        BeginDrawing();//starts drawing
        ClearBackground(RAYWHITE);//clears background

        //draws the background texture to the screen
        for(int i = 0; i < screenWidth; i += 32)
        {
            for(int j = 0; j < screenHeight; j += 32)
            {
                DrawTexturePro(
                    backgroundTexture,
                    (Rectangle) {0.0f, 0.0f, 16.0f, 16.0f},
                    (Rectangle) {
                        (float) i,
                        (float) j,
                        32.0f,
                        32.0f
                    },
                    (Vector2) {0.0f, 0.0f},
                    0.0f,
                    WHITE
                );
            }
        }

        DrawText(
            "this is some text",
            2 * boardMenu.scale,
            2 * boardMenu.scale,
            7 * boardMenu.scale,
            BLACK
        );

        BeginMode2D(worldCamera);//sets camera to worldCamera
        /* NOTE: WORLD DRAWING SECTION */

        //draws outline around logic board
        DrawRectangle(
            -4, -4, (mainBoard.w << 3) + 8, (mainBoard.h << 3) + 8,
            (Color) {0x26, 0x26, 0x28, 0xff}
        );

        //draws the logic board to the screen
        DisplayLogicBoard(mainBoard);

        //draws where the selection has been made
        if(boardMenu.clickTracker[2])
            DrawRectangle(
                mainCb.x << 3,
                mainCb.y << 3,
                mainCb.w << 3,
                mainCb.h << 3,
                (Color) {255, 255, 255, 100}
            );

        //if the mouse is not over the menu
        if(!CheckCollisionPointRec(
            GetMousePosition(),
            (Rectangle) {
                boardMenu.position.x, boardMenu.position.y,
                boardMenu.menuBounds.width * boardMenu.scale,
                boardMenu.menuBounds.height * boardMenu.scale
            }
        ))
        {
            //gets the pos of the mouse in the world
            Vector2 mouseInWorldPos = GetScreenToWorld2D(
                GetMousePosition(),
                worldCamera
            );

            //if the mouse is over the logic board
            if(CheckCollisionPointRec(
                mouseInWorldPos,
                (Rectangle) {
                    0.0f,
                    0.0f,
                    (float)(mainBoard.w << 3),
                    (float)(mainBoard.h << 3)
                }
            ))
            {
                //draws a marker showing where the mouse is on the board
                DrawTexturePro(
                    menuTexture,
                    (Rectangle) {
                        101.0f,
                        //makes the marker red if the mouse is down and has not
                        //moved by a significant amount
                        (
                            IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
                            !worldCamWasDragged &&
                            !boardMenu.menuClicked
                        )? 10.0f : 0.0f,
                        10.0f,
                        10.0f
                    },
                    (Rectangle) {
                        -1.0f + floorf(mouseInWorldPos.x / 8.0f) * 8.0f,
                        -1.0f + floorf(mouseInWorldPos.y / 8.0f) * 8.0f,
                        10.0f,
                        10.0f
                    },
                    (Vector2) {0.0f, 0.0f},
                    0.0f,
                    WHITE
                );

            }
        }

        /* WORLD DRAWING SECTION END*/
        EndMode2D();//stops using world camera
        /* NOTE:  STATIC DRAWING SECTION (HUD, UI, etc...)*/

        //draws the menu to the screen

        //loops over the elements in the menu
        for(int i = 0; i < boardMenu.elemNum; i++)
        {
            //draws the menu texture
            DrawTexturePro(
                boardMenu.srcImg,
                (Rectangle) {
                    boardMenu.elemSrcs[i].x,

                    //if some of the elements are clicked, the source is moved
                    boardMenu.elemSrcs[i].y + 
                        ((((i < 5)? 42.0f : 0.0f) +
                        (((i < 13) && (i > 6)) ? 12.0f : 0.0f)) *
                        ((float)(boardMenu.clickTracker[i] & 1))),

                    boardMenu.elemSrcs[i].width,
                    boardMenu.elemSrcs[i].height,
                },

                (Rectangle) {
                    boardMenu.position.x +
                        (boardMenu.scale * boardMenu.elemPos[i].x),
                    boardMenu.position.y +
                        (boardMenu.scale * boardMenu.elemPos[i].y),
                    boardMenu.scale * boardMenu.elemPos[i].width,
                    boardMenu.scale * boardMenu.elemPos[i].height
                },

                //chenges offset for elem 1
                (i == 1)?
                    (Vector2) {
                        5.0f * boardMenu.scale,
                        6.0f * boardMenu.scale
                    } :
                    (Vector2) {0.0f, 0.0f},

                //changes rotation of elem 1 based on clickTracker
                (i == 1)?
                    90.0f * ((float) (boardMenu.clickTracker[i] & 2)) : 0.0f,

                WHITE
            );

            //if window should be minimized, skips displaying some elems
            if((i == 4) &&
                (boardMenu.clickTracker[4] ||
                boardMenu.clickTracker[2])
            )i = 6;
            if((i == 12) &&
                (boardMenu.clickTracker[4] ||
                boardMenu.clickTracker[2])
            )break;

            //if the bottom menu is hidden
            if((i == 6) && !boardMenu.clickTracker[3])break;
        }

        //if the menu is not minimized
        if(!(boardMenu.clickTracker[4] || boardMenu.clickTracker[2]))
        {
            //draws marker of where the mouse is over
            if(mouseOnMenuPos.x > -0.5f)
                DrawTexturePro(
                    menuTexture,
                    (Rectangle) {
                        101.0f,
                        (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
                        (boardMenu.clickTracker[6] ==
                        boardMenu.clickTracker[5])
                    )? 10.0f : 0.0f,
                        10.0f,
                        10.0f
                    },
                    (Rectangle) {
                        boardMenu.position.x +
                        mouseOnMenuPos.x * boardMenu.scale,
                        boardMenu.position.y +
                        mouseOnMenuPos.y * boardMenu.scale,
                        10.0f * boardMenu.scale,
                        10.0f * boardMenu.scale
                    },
                    (Vector2){0.0f, 0.0f},
                    0.0f,
                    WHITE
                );

            //draws the marker of what type is selected
            if(selectedPos.x > -0.5f)
                DrawTexturePro(
                    menuTexture,
                    (Rectangle) {101.0f, 10.0f, 10.0f, 10.0f},
                    (Rectangle) {
                        boardMenu.position.x +
                        (selectedPos.x - 1.0f) * boardMenu.scale,
                        boardMenu.position.y +
                        (selectedPos.y - 1.0f) * boardMenu.scale,
                        10.0f * boardMenu.scale,
                        10.0f * boardMenu.scale
                    },
                    (Vector2){0.0f, 0.0f},
                    0.0f,
                    WHITE
                );
        }

        //draws the power selection part of the menu
        if(
            boardMenu.clickTracker[3] &&
            !boardMenu.clickTracker[2] &&
            !boardMenu.clickTracker[4]
        )
        {
            //loops over inputs
            for(int i = 0; i < 16; i++)
            {

                //draws state of input
                DrawTexturePro(
                    menuTexture,
                    (Rectangle) {
                        51.0f + ((i & 7) * 6),
                        84.0f + (((i >> 3) & 1) * 12) + (
                            5 * ((mainBoard.input >> i) & 1)
                        ),
                        6.0f,
                        5.0f
                    },
                    (Rectangle) {
                        boardMenu.position.x +
                            ((51.0f + ((float)(i * 6))) * boardMenu.scale),
                        boardMenu.position.y + (42.0f * boardMenu.scale),
                        6.0f * boardMenu.scale,
                        5.0f * boardMenu.scale
                    },
                    (Vector2) {0.0f, 0.0f},
                    0.0f,
                    WHITE
                );

                //draws input change button
                DrawTexturePro(
                    menuTexture,
                    (Rectangle) {
                        105.0f,
                        94.0f + (7 * ((mainBoard.input >> i) & 1)),
                        6.0f,
                        7.0f
                    },
                    (Rectangle) {
                        boardMenu.position.x +
                            ((51.0f + ((float)(i * 6))) * boardMenu.scale),
                        boardMenu.position.y + (47.0f * boardMenu.scale),
                        6.0f * boardMenu.scale,
                        7.0f * boardMenu.scale
                    },
                    (Vector2) {0.0f, 0.0f},
                    0.0f,
                    WHITE
                );
            }
        }

        /* STATIC DRAWING SECTION END*/
        EndDrawing();//stops drawing
    }

    /* NOTE: GAME EXIT SECTION*/

    //frees allocated mem
    UnloadLogicBoardTextures();
    UnloadTexture(menuTexture);
    UnloadTexture(backgroundTexture);
    FreeLogicBoard(&mainBoard);
    FreeLogicBoard(&mainCb.board);

    //does what it says
    CloseWindow();

    //end of main function
    return 0;
}

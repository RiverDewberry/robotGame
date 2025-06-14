#include "../../include/logicBoard.h"
#include "../../include/inputHandler.h"
#include "../../include/strutils.h"
#include "../../include/screens.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

void DrawBox(float x, float y, float w, float h, float scale)
{
    x *= scale;
    y *= scale;
    w *= scale;
    h *= scale;

    DrawRectangle(
        x, y, w, h,
        (Color) {0x78, 0x79, 0x7d, 0xff}
    );

    DrawRectangle(
        x, y - scale, w, scale,
        (Color) {0x1b, 0x1b, 0x1d, 0xff}
    );

    DrawRectangle(
        x - scale, y, scale, h,
        (Color) {0x1b, 0x1b, 0x1d, 0xff}
    );

    DrawRectangle(
        x, y + h, w, scale,
        (Color) {0x3c, 0x3d, 0x3f, 0xff}
    );

    DrawRectangle(
        x + w, y, scale, h,
        (Color) {0x3c, 0x3d, 0x3f, 0xff}
    );
}

void DrawTextInput(TextInput ti, float scale)
{

    DrawRectangle(
        ti.bounds.x, ti.bounds.y, ti.bounds.width, ti.bounds.height,
        ti.mouseOnInput ?
            (Color) {0xc6, 0xa1, 0x53, 0xff} :
            (Color) {0x4e, 0x40, 0x28, 0xff}
    );

    DrawRectangle(
        ti.bounds.x + scale, ti.bounds.y + scale,
        ti.bounds.width - scale * 2, ti.bounds.height - scale * 2,
        ti.isSelected ?
            (Color) {0xc6, 0xa1, 0x53, 0xff} :
            (Color) {0x4e, 0x40, 0x28, 0xff}
    );

    if((ti.curIndex < ti.maxLen) && ti.isSelected && ti.mouseOnInput)
    {
        ti.textOutput[ti.curIndex] = '_';
        ti.textOutput[ti.curIndex + 1] = 0;
    }

    DrawText(
        ti.textOutput,
        ti.bounds.x + scale * 2,
        ti.bounds.y + scale * 2,
        7.0f * scale,
        ti.isSelected ?
            (Color) {0x4e, 0x40, 0x28, 0xff} :
            (Color) {0xc6, 0xa1, 0x53, 0xff}
    );

    if((ti.curIndex < ti.maxLen) && ti.isSelected && ti.mouseOnInput)
    {
        ti.textOutput[ti.curIndex] = 0;
    }
}

void DrawButton(Button bn, float scale, const char *text)
{

    DrawRectangle(
        bn.bounds.x * scale,
        bn.bounds.y * scale,
        bn.bounds.width * scale,
        bn.bounds.height * scale,
        bn.mouseOnInput ?
            (Color) {0xc6, 0xa1, 0x53, 0xff} :
            (Color) {0x4e, 0x40, 0x28, 0xff}
    );

    DrawRectangle(
        bn.bounds.x * scale + scale, bn.bounds.y * scale + scale,
        (bn.bounds.width - 2.0f) * scale, (bn.bounds.height - 2.0f) * scale,
        bn.isSelected ?
            (Color) {0xc6, 0xa1, 0x53, 0xff} :
            (Color) {0x4e, 0x40, 0x28, 0xff}
    );

    DrawText(
        text,
        (bn.bounds.x + 2) * scale,
        (bn.bounds.y + 2) * scale,
        7.0f * scale,
        bn.isSelected ?
            (Color) {0x4e, 0x40, 0x28, 0xff} :
            (Color) {0xc6, 0xa1, 0x53, 0xff}
    );
}

void DrawSelectionList(SelectionList sl, char** labels)
{
    for(int i = 0; i < sl.elemsShown; i++)
    {
        DrawRectangle(
            (sl.bounds.x + i * sl.offset.x) * sl.scale,
            (sl.bounds.y + i * sl.offset.y) * sl.scale,
            sl.bounds.width * sl.scale,
            sl.bounds.height * sl.scale,
            (sl.elemMouseOn == (i + sl.firstElemShown)) ?
                (Color) {0xc6, 0xa1, 0x53, 0xff} :
                (Color) {0x4e, 0x40, 0x28, 0xff}
        );

        if((i + sl.firstElemShown) >= sl.elemNums) continue;

        DrawRectangle(
            (sl.bounds.x + i * sl.offset.x + 1.0f) * sl.scale,
            (sl.bounds.y + i * sl.offset.y + 1.0f) * sl.scale,
            (sl.bounds.width - 2.0f) * sl.scale,
            (sl.bounds.height - 2.0f) * sl.scale,
            (sl.elemSelected == (i + sl.firstElemShown)) ?
                (Color) {0xc6, 0xa1, 0x53, 0xff} :
                (Color) {0x4e, 0x40, 0x28, 0xff}
        );

        DrawText(
            (
                ((sl.elemsShown > 1) &&
                ((i + 1) == sl.elemsShown) &&
                !((i + sl.firstElemShown + 1) >= sl.elemNums)) ||
                ((i == 0) && (sl.firstElemShown != 0) && (sl.elemsShown > 2))
            ) ?
                "..." :
                GetFileNameWithoutExt(labels[i + sl.firstElemShown]),
            (sl.bounds.x + i * sl.offset.x + 2.0f) * sl.scale,
            (sl.bounds.y + i * sl.offset.y + 2.0f) * sl.scale,
            7.0f * sl.scale,
            (sl.elemSelected == (i + sl.firstElemShown)) ?
                (Color) {0x4e, 0x40, 0x28, 0xff} :
                (Color) {0xc6, 0xa1, 0x53, 0xff}
        );
    }
}

void SwapBoard(char* newBoardPath, LogicBoard *lb)
{
    FreeLogicBoard(lb);
    *lb = LoadLogicBoard(newBoardPath);
    UpdatePowerLogicBoard(lb);
}

//returns 1 if lb changed
void ResizeBoard(LogicBoard *lb, int newWidth, int newHeight)
{
    LogicBoard temp = NewLogicBoard(newWidth, newHeight);

    int maxWidth = newWidth;
    int maxHeight = newHeight;

    if(maxWidth > lb->w)maxWidth = lb->w;
    if(maxHeight > lb->h)maxHeight = lb->h;

    for(int i = 0; i < maxHeight; i++)
    {
        for(int j = 0; j < maxWidth; j++)
        {
            temp.board[j + i * temp.w] = lb->board[j + i * lb->w];
        }
    }

    FreeLogicBoard(lb);

    *lb = temp;

    return;
}

ScreenState SelectLogicBoard(char **outPath)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    //used to scale the ui
    float uiScale = ((float) screenWidth) / 400.0f;
    if(uiScale < 1.0f)uiScale = 1.0f;
    if(uiScale > 2.5f)uiScale = 2.5f;
    uiScale *= 7.5f;
    uiScale = floorf(uiScale);
    uiScale *= 0.2f;

    //currently selected logic board, starts empty
    LogicBoard selectedBoard = NewLogicBoard(1, 1);

    //the main camera used for displaying the game world
    Camera2D worldCamera = {
        .target = (Vector2){
            (float)(selectedBoard.w << 2),
            (float)(selectedBoard.h << 2)
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
            (float)(selectedBoard.w << 3),
            (float)(selectedBoard.h << 3)
        },
        &worldCamera,
        -1.0f,
        (Vector2){0.0f, 0.0f},
        (Vector2){0.0f, 0.0f}
    };

    //if the menu was clicked
    char menuClicked = 0;

    //if the mouse is over the menu
    char mouseOnMenu = 0;

    //area where the menu is
    Rectangle menuArea = (Rectangle) {
        0, 0,
        100 * uiScale,
        screenHeight
    };

    char *filePathStr = AddPath("/sprites/boardBackground.png");
    Texture2D backgroundTexture = LoadTexture(filePathStr);
    AddPathReuse("/boards", &filePathStr);
    FilePathList boardPaths = LoadDirectoryFiles(filePathStr);

    TextInput widthInput = {
        .textOutput = (char[]) {'0', 0, 0, 0},
        .maxLen = 3,
        .bounds = {0, 0, 0, 0},
        .curIndex = 0,
        .isSelected = 0,
        .wasClicked = 0,
        .mouseOnInput = 0,
        .inputMode = NUMBERS
    };

    TextInput heightInput = {
        .textOutput = (char[]) {'0', 0, 0, 0},
        .maxLen = 3,
        .bounds = {0, 0, 0, 0},
        .curIndex = 0,
        .isSelected = 0,
        .wasClicked = 0,
        .mouseOnInput = 0,
        .inputMode = NUMBERS
    };

    TextInput nameInput = {
        .textOutput = (char[]) 
            {
                'E', 'n', 't', 'e', 'r', ' ', 'n', 'a', 'm', 'e',
                0, 0, 0, 0, 0, 0
            },
        .maxLen = 15,
        .bounds = {0, 0, 0, 0},
        .curIndex = 0,
        .isSelected = 0,
        .wasClicked = 0,
        .mouseOnInput = 0,
        .inputMode = LETTERS | DASHES | NUMBERS
    };

    Button createBoardButton = {
        (Rectangle) {3.0f, 15.0f, 94.0f, 11.0f}, 0, 0, 0
    };

    Button selectButton = {
        (Rectangle) {3.0f, 66.0f, 31.0f, 11.0f}, 0, 0, 0
    };

    Button deleteButton = {
        (Rectangle) {35.0f, 66.0f, 31.0f, 11.0f}, 0, 0, 0
    };

    Button editButton = {
        (Rectangle) {67.0f, 66.0f, 30.0f, 11.0f}, 0, 0, 0
    };

    SelectionList fileList = {
        .bounds = (Rectangle) {3.0f, 81.0f, 94.0f, 11.0f},
        .offset = (Vector2) {0.0f, 12.0f},
        .elemClicked = -1,
        .scale = 1.0f,
        .elemMouseOn = -1,
        .elemSelected = -1,
        .elemNums = boardPaths.count,
        .elemsShown = 1,
        .firstElemShown = 0,
        .scroll = 0.0f
    };

    int loadedBoardNum = -1;

    LoadLogicBoardTextures();

    char fileMade = 0;

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

        //updates menu scale based on screen size
        uiScale = ((float) screenWidth) / 400.0f;
        if(uiScale < 1.0f)uiScale = 1.0f;
        if(uiScale > 2.5f)uiScale = 2.5f;
        uiScale *= 7.5f;
        uiScale = floorf(uiScale);
        uiScale *= 0.2f;

        //area where the menu is
        menuArea = (Rectangle) {
            0, 0,
            100 * uiScale,
            screenHeight
        };

        widthInput.bounds = (Rectangle) {
            75.0f * uiScale,
            27.0f * uiScale,
            22.0f * uiScale,
            11.0f * uiScale
        };

        heightInput.bounds = (Rectangle) {
            75.0f * uiScale,
            39.0f * uiScale,
            22.0f * uiScale,
            11.0f * uiScale
        };

        nameInput.bounds = (Rectangle) {
            3.0f * uiScale,
            51.0f * uiScale,
            94.0f * uiScale,
            11.0f * uiScale
        };

        mouseOnMenu = CheckCollisionPointRec(
            GetMousePosition(),
            menuArea
        );

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            menuClicked = mouseOnMenu;
        }

        if(!mouseOnMenu)
        {
            //updates world camera zoom based on mouse zoom
            ZoomCamera(&worldCamera, 1.0f, 64.0f, 0.05f);
        } else
        {
            float temp = GetMouseWheelMoveV().y;

            if(temp != 0.0f)
            {
                ScrollSelectionList(
                    &fileList, temp * 5.0f
                );

                if(
                    (fileList.elemSelected < fileList.firstElemShown) ||
                    (fileList.elemSelected >= (
                    fileList.firstElemShown + fileList.elemsShown))
                )
                    fileList.elemSelected  = -1;

                if(
                    (fileList.firstElemShown != 0) &&
                    (fileList.firstElemShown == fileList.elemSelected) &&
                    (fileList.elemsShown > 2)
                )
                    fileList.elemSelected  = -1;
            }
        }

        if(!menuClicked)
        {
            //updates position of background
            UpdateDragTracker(&worldCamDrag);
        }

        //updates file list scale and elemsShown
        fileList.scale = uiScale;

        int fileListDisplayHeight = ((int) (GetScreenHeight() / uiScale)) - 82;
        if(fileListDisplayHeight < 13) fileListDisplayHeight = 13;

        fileList.elemsShown = (fileListDisplayHeight - 1) / 12;
        if(fileList.elemsShown < 1) fileList.elemsShown = 1;

        fileList.elemNums = boardPaths.count;

        //updates ui elements
        UpdateTextInput(&widthInput);
        UpdateTextInput(&heightInput);
        UpdateTextInput(&nameInput);
        UpdateButtonScaled(&createBoardButton, uiScale);
        UpdateButtonScaled(&editButton, uiScale);
        UpdateButtonScaled(&deleteButton, uiScale);
        UpdateButtonScaled(&selectButton, uiScale);
        UpdateSelectionList(&fileList);

        //updates the file list selection if some part of the list is not shown
        //on screen
        if(
            fileList.elemsShown != 1 &&
            ((fileList.firstElemShown + fileList.elemsShown) <
            fileList.elemNums)
        )
        {
            if(
                fileList.elemSelected ==
                (fileList.firstElemShown + fileList.elemsShown - 1)
            )
                fileList.elemSelected = -1;

            if(
                fileList.elemMouseOn ==
                (fileList.firstElemShown + fileList.elemsShown - 1)
            )
            {
                if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    fileList.elemMouseOn = -1;
                    fileList.firstElemShown++;
                }
            }
        }

        if(
            ((fileList.firstElemShown != 0) && (fileList.elemsShown > 2)) &&
            (fileList.elemMouseOn == fileList.firstElemShown)
        )
        {
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                fileList.elemMouseOn = -1;
                fileList.elemSelected = -1;
                fileList.firstElemShown--;
            }
        }

        //updates width and height inputs
        if((widthInput.curIndex == 1) && (widthInput.textOutput[0] == '0'))
            widthInput.curIndex = 0;
        if((heightInput.curIndex == 1) && (heightInput.textOutput[0] == '0'))
            heightInput.curIndex = 0;
        if(widthInput.curIndex == 0)
            *widthInput.textOutput = '1';
        if(heightInput.curIndex == 0)
            *heightInput.textOutput = '1';

        if(GetNumFromStr(widthInput.textOutput) > 250)
            SetStrToNum(widthInput.textOutput, 250);
        if(GetNumFromStr(heightInput.textOutput) > 250)
            SetStrToNum(heightInput.textOutput, 250);

        if(
            (GetNumFromStr(widthInput.textOutput) != selectedBoard.w) ||
            (GetNumFromStr(heightInput.textOutput) != selectedBoard.h)
        )
        {
            if(fileList.elemSelected != -1)
            {
                SwapBoard(
                    boardPaths.paths[loadedBoardNum],
                    &selectedBoard
                );
            }

            ResizeBoard(
                &selectedBoard,
                GetNumFromStr(widthInput.textOutput),
                GetNumFromStr(heightInput.textOutput)
            );

            worldCamDrag.bounds = (Rectangle) {
                0.0f,
                0.0f,
                (float)(selectedBoard.w << 3),
                (float)(selectedBoard.h << 3)
            };

            BoundDragTracker(&worldCamDrag);
        }

        //updates name input
        if(nameInput.curIndex == 0)
        {
            strcpy(nameInput.textOutput, "Enter name");

            if(
                createBoardButton.mouseOnInput &&
                IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            )
                nameInput.isSelected = 1;

            createBoardButton.isSelected = 0;
        }

        //prevents button inputs if no file selected
        if(fileList.elemSelected == -1)
        {
            editButton.mouseOnInput = 0;
            editButton.isSelected = 0;
            selectButton.mouseOnInput = 0;
            selectButton.isSelected = 0;
            deleteButton.mouseOnInput = 0;
            deleteButton.isSelected = 0;
        }

        if(editButton.isSelected)
        {

            char *tempFileName = (char*) malloc(
                strlen(
                    GetFileName(boardPaths.paths[loadedBoardNum])
                ) + 9
            );

            strcpy(tempFileName, "/boards/");

            strcpy(
                tempFileName + 8,
                GetFileName(boardPaths.paths[loadedBoardNum])
            );

            ScreenState temp = EditLogicBoard(
                tempFileName
            );

            free(tempFileName);

            if(temp == EXITAPP)
            {
                free(filePathStr);
                UnloadDirectoryFiles(boardPaths);
                FreeLogicBoard(&selectedBoard);
                UnloadLogicBoardTextures();
                UnloadTexture(backgroundTexture);

                return EXITAPP;
            } else
            {
                SwapBoard(
                    boardPaths.paths[loadedBoardNum],
                    &selectedBoard
                );

                SetStrToNum(widthInput.textOutput, selectedBoard.w);
                SetStrToNum(heightInput.textOutput, selectedBoard.h);

                int i;

                for(i = 0; widthInput.textOutput[i] != 0; i++);

                widthInput.curIndex = i;

                for(i = 0; heightInput.textOutput[i] != 0; i++);

                heightInput.curIndex = i;

                strcpy(
                    nameInput.textOutput,
                    GetFileNameWithoutExt(boardPaths.paths[loadedBoardNum])
                );

                nameInput.curIndex = strlen(
                    GetFileNameWithoutExt(boardPaths.paths[loadedBoardNum])
                );

                worldCamDrag.bounds = (Rectangle) {
                    0.0f,
                    0.0f,
                    (float)(selectedBoard.w << 3),
                    (float)(selectedBoard.h << 3)
                };
            }
        }

        if(deleteButton.isSelected)
        {
            remove(boardPaths.paths[fileList.elemSelected]);

            fileList.elemSelected = -1;

            //reloads files in the fileList
            UnloadDirectoryFiles(boardPaths);
            AddPathReuse("/boards", &filePathStr);
            boardPaths = LoadDirectoryFiles(filePathStr);
            fileList.elemNums = boardPaths.count;
        }

        if(createBoardButton.isSelected && fileMade)
        {
            fileMade = 0;
            char *tempFileName = (char*) malloc(
                strlen(nameInput.textOutput) + 13
            );

            strcpy(tempFileName, "/boards/");

            strcpy(
                tempFileName + 8,
                nameInput.textOutput
            );

            strcpy(
                tempFileName + 8 + strlen(nameInput.textOutput),
                ".bin"
            );

            AddPathReuse(tempFileName, &filePathStr);

            if(fileList.elemSelected != -1)
                remove(boardPaths.paths[fileList.elemSelected]);

            StoreLogicBoard(
                filePathStr,
                selectedBoard
            );

            free(tempFileName);

            //reloads files in the fileList
            UnloadDirectoryFiles(boardPaths);
            AddPathReuse("/boards", &filePathStr);
            boardPaths = LoadDirectoryFiles(filePathStr);
            fileList.elemNums = boardPaths.count;

            fileList.elemSelected = -1;

            for(int i = 0; i < boardPaths.count; i++)
            {
                if(strcmp(
                   nameInput.textOutput,
                   GetFileNameWithoutExt(boardPaths.paths[i])
                ) == 0)
                {
                    fileList.elemSelected = i;
                    break;
                }
            }

            if(fileList.elemSelected != -1)
            {
                fileList.firstElemShown = fileList.elemSelected;

                if((fileList.elemNums > 2) && (fileList.firstElemShown != 0))
                {
                    fileList.firstElemShown--;
                }
            }

        } else if(!createBoardButton.isSelected)
            fileMade = 1;

        //makes sure that list does not show elements after the bottom of the
        //if there are enough elems to fill list
        if(
            (fileList.firstElemShown + fileList.elemsShown) >
            fileList.elemNums
        )
            fileList.firstElemShown = fileList.elemNums - fileList.elemsShown;

        if(fileList.firstElemShown < 0)fileList.firstElemShown = 0;


        //if the selected board changes
        if(loadedBoardNum != fileList.elemSelected)
        {
            loadedBoardNum = fileList.elemSelected;

            if(loadedBoardNum != -1)
            {
                SwapBoard(
                    boardPaths.paths[loadedBoardNum],
                    &selectedBoard
                );

                SetStrToNum(widthInput.textOutput, selectedBoard.w);
                SetStrToNum(heightInput.textOutput, selectedBoard.h);

                int i;

                for(i = 0; widthInput.textOutput[i] != 0; i++);

                widthInput.curIndex = i;

                for(i = 0; heightInput.textOutput[i] != 0; i++);

                heightInput.curIndex = i;

                strcpy(
                    nameInput.textOutput,
                    GetFileNameWithoutExt(boardPaths.paths[loadedBoardNum])
                );

                nameInput.curIndex = strlen(
                    GetFileNameWithoutExt(boardPaths.paths[loadedBoardNum])
                );

                worldCamDrag.bounds = (Rectangle) {
                    0.0f,
                    0.0f,
                    (float)(selectedBoard.w << 3),
                    (float)(selectedBoard.h << 3)
                };
            } else
            {
                //resets inputs
                SetStrToNum(widthInput.textOutput, 1);
                SetStrToNum(heightInput.textOutput, 1);
                widthInput.curIndex = 0;
                heightInput.curIndex = 0;
                nameInput.curIndex = 0;
                nameInput.textOutput[0] = 0;
                FreeLogicBoard(&selectedBoard);
                selectedBoard = NewLogicBoard(1, 1);
                worldCamDrag.bounds = (Rectangle) {
                    0.0f,
                    0.0f,
                    (float)(selectedBoard.w << 3),
                    (float)(selectedBoard.h << 3)
                };
            }
        }

        //sends outout to output var and returns from screen
        if(selectButton.isSelected)
        {
            if((outPath != NULL)){

                *outPath = malloc(
                    sizeof(boardPaths.paths[fileList.elemSelected]) + 1
                );

                strcpy(*outPath, boardPaths.paths[fileList.elemSelected]);
            }
            return LEFTSCREEN;
        }

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

        BeginMode2D(worldCamera);//sets camera to worldCamera
        /* NOTE: WORLD DRAWING SECTION */

        DisplayLogicBoard(selectedBoard, worldCamera);

        EndMode2D();//stops using world camera
        /* NOTE:  STATIC DRAWING SECTION (HUD, UI, etc...)*/

        DrawRectangleRec(menuArea, (Color) {0x26, 0x26, 0x28, 0xff});

        DrawBox(2, 2, 96, 10, uiScale);
        DrawBox(2, 14, 96, 49, uiScale);
        DrawBox(2, 65, 96, 13, uiScale);
        DrawBox(2, 80, 96, fileList.elemsShown * 12 + 1, uiScale);

        DrawText(
            "Board selection",
            3.0f * uiScale,
            2.0f * uiScale,
            10.0f * uiScale,
            (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && menuClicked) ?
                (Color) {0xc6, 0xa1, 0x53, 0xff} :
                (Color) {0x4e, 0x40, 0x28, 0xff}
        );

        DrawButton(
            createBoardButton,
            uiScale,
            fileList.elemSelected == -1 ?
                "Create new board" :
                "Apply changes"
        );

        DrawButton(
            editButton,
            uiScale,
            (fileList.elemSelected != -1) ? "Edit" : ""
        );

        DrawButton(
            deleteButton,
            uiScale,
            (fileList.elemSelected != -1) ? "Delete" : ""
        );

        DrawButton(
            selectButton,
            uiScale,
            (fileList.elemSelected != -1) ? "Select" : ""
        );

        DrawText(
            "Enter width ->",
            4.0f * uiScale,
            29.0f * uiScale,
            7.0f * uiScale,
            widthInput.isSelected ?
                (Color) {0xc6, 0xa1, 0x53, 0xff} :
                (Color) {0x4e, 0x40, 0x28, 0xff}
        );

        DrawText(
            "Enter height ->",
            4.0f * uiScale,
            41.0f * uiScale,
            7.0f * uiScale,
            heightInput.isSelected ?
                (Color) {0xc6, 0xa1, 0x53, 0xff} :
                (Color) {0x4e, 0x40, 0x28, 0xff}
        );

        DrawTextInput(widthInput, uiScale);
        DrawTextInput(heightInput, uiScale);
        DrawTextInput(nameInput, uiScale);

        DrawSelectionList(fileList, boardPaths.paths);

        EndDrawing();//draws to screen
    }

    free(filePathStr);
    UnloadDirectoryFiles(boardPaths);
    FreeLogicBoard(&selectedBoard);
    UnloadLogicBoardTextures();
    UnloadTexture(backgroundTexture);

    return EXITAPP;
}
